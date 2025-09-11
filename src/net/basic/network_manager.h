#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QJsonObject>

namespace shogi::net {

/**
 * @brief Basic network manager for online Shogi games
 */
class NetworkManager : public QObject {
    Q_OBJECT

public:
    enum class Role {
        None,
        Server,
        Client
    };

    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();
    
    // Server mode
    bool startServer(quint16 port = 12345);
    void stopServer();
    
    // Client mode
    bool connectToServer(const QString& host, quint16 port = 12345);
    void disconnectFromServer();
    
    // Communication
    void sendMessage(const QJsonObject& message);
    void sendMove(const QString& move);
    void sendGameState(const QString& sfen);
    
    // Status
    bool isConnected() const;
    Role getRole() const { return role_; }

signals:
    void connected();
    void disconnected();
    void messageReceived(const QJsonObject& message);
    void moveReceived(const QString& move);
    void gameStateReceived(const QString& sfen);
    void errorOccurred(const QString& error);

private slots:
    void onNewConnection();
    void onClientConnected();
    void onClientDisconnected();
    void onDataReceived();
    void onSocketError();

private:
    void processMessage(const QByteArray& data);
    void sendData(const QByteArray& data);
    
    QTcpServer* server_;
    QTcpSocket* socket_;
    Role role_;
    QByteArray buffer_;
};

} // namespace shogi::net