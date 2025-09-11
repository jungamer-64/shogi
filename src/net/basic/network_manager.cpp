#include "network_manager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace shogi::net {

NetworkManager::NetworkManager(QObject* parent) 
    : QObject(parent), server_(nullptr), socket_(nullptr), role_(Role::None) {
}

NetworkManager::~NetworkManager() {
    stopServer();
    disconnectFromServer();
}

bool NetworkManager::startServer(quint16 port) {
    if (server_) {
        stopServer();
    }
    
    server_ = new QTcpServer(this);
    connect(server_, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);
    
    if (!server_->listen(QHostAddress::Any, port)) {
        delete server_;
        server_ = nullptr;
        emit errorOccurred("Failed to start server: " + server_->errorString());
        return false;
    }
    
    role_ = Role::Server;
    qDebug() << "Server started on port" << port;
    return true;
}

void NetworkManager::stopServer() {
    if (server_) {
        server_->close();
        delete server_;
        server_ = nullptr;
    }
    
    if (socket_ && role_ == Role::Server) {
        socket_->disconnectFromHost();
        socket_->deleteLater();
        socket_ = nullptr;
    }
    
    role_ = Role::None;
}

bool NetworkManager::connectToServer(const QString& host, quint16 port) {
    if (socket_) {
        disconnectFromServer();
    }
    
    socket_ = new QTcpSocket(this);
    connect(socket_, &QTcpSocket::connected, this, &NetworkManager::onClientConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &NetworkManager::onClientDisconnected);
    connect(socket_, &QTcpSocket::readyRead, this, &NetworkManager::onDataReceived);
    connect(socket_, &QTcpSocket::errorOccurred, this, &NetworkManager::onSocketError);
    
    socket_->connectToHost(host, port);
    
    if (!socket_->waitForConnected(5000)) {
        emit errorOccurred("Failed to connect to server: " + socket_->errorString());
        socket_->deleteLater();
        socket_ = nullptr;
        return false;
    }
    
    role_ = Role::Client;
    return true;
}

void NetworkManager::disconnectFromServer() {
    if (socket_) {
        socket_->disconnectFromHost();
        socket_->deleteLater();
        socket_ = nullptr;
    }
    
    if (role_ == Role::Client) {
        role_ = Role::None;
    }
}

void NetworkManager::sendMessage(const QJsonObject& message) {
    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";
    sendData(data);
}

void NetworkManager::sendMove(const QString& move) {
    QJsonObject message;
    message["type"] = "move";
    message["move"] = move;
    sendMessage(message);
}

void NetworkManager::sendGameState(const QString& sfen) {
    QJsonObject message;
    message["type"] = "gamestate";
    message["sfen"] = sfen;
    sendMessage(message);
}

bool NetworkManager::isConnected() const {
    return socket_ && socket_->state() == QTcpSocket::ConnectedState;
}

void NetworkManager::onNewConnection() {
    if (!server_) {
        return;
    }
    
    if (socket_) {
        // Already have a connection, reject new one
        QTcpSocket* newSocket = server_->nextPendingConnection();
        newSocket->disconnectFromHost();
        newSocket->deleteLater();
        return;
    }
    
    socket_ = server_->nextPendingConnection();
    connect(socket_, &QTcpSocket::disconnected, this, &NetworkManager::onClientDisconnected);
    connect(socket_, &QTcpSocket::readyRead, this, &NetworkManager::onDataReceived);
    connect(socket_, &QTcpSocket::errorOccurred, this, &NetworkManager::onSocketError);
    
    emit connected();
    qDebug() << "Client connected from" << socket_->peerAddress().toString();
}

void NetworkManager::onClientConnected() {
    emit connected();
    qDebug() << "Connected to server";
}

void NetworkManager::onClientDisconnected() {
    socket_->deleteLater();
    socket_ = nullptr;
    
    emit disconnected();
    qDebug() << "Disconnected";
}

void NetworkManager::onDataReceived() {
    if (!socket_) {
        return;
    }
    
    buffer_.append(socket_->readAll());
    
    // Process complete messages (separated by newlines)
    int newlineIndex;
    while ((newlineIndex = buffer_.indexOf('\n')) != -1) {
        QByteArray messageData = buffer_.left(newlineIndex);
        buffer_.remove(0, newlineIndex + 1);
        
        if (!messageData.isEmpty()) {
            processMessage(messageData);
        }
    }
}

void NetworkManager::onSocketError() {
    if (socket_) {
        emit errorOccurred("Socket error: " + socket_->errorString());
    }
}

void NetworkManager::processMessage(const QByteArray& data) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        emit errorOccurred("JSON parse error: " + error.errorString());
        return;
    }
    
    QJsonObject message = doc.object();
    emit messageReceived(message);
    
    QString type = message["type"].toString();
    if (type == "move") {
        emit moveReceived(message["move"].toString());
    } else if (type == "gamestate") {
        emit gameStateReceived(message["sfen"].toString());
    }
}

void NetworkManager::sendData(const QByteArray& data) {
    if (socket_ && socket_->state() == QTcpSocket::ConnectedState) {
        socket_->write(data);
        socket_->flush();
    }
}

} // namespace shogi::net

#include "network_manager.moc"