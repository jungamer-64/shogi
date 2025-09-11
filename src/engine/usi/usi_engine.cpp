#include "usi_engine.h"
#include <QDebug>
#include <QTextStream>

namespace shogi::engine {

USIEngine::USIEngine(QObject* parent) 
    : QObject(parent), engineProcess_(nullptr), commandTimer_(new QTimer(this)), isReady_(false) {
    
    commandTimer_->setSingleShot(true);
    commandTimer_->setInterval(5000);  // 5 second timeout
    connect(commandTimer_, &QTimer::timeout, this, &USIEngine::onCommandTimeout);
}

USIEngine::~USIEngine() {
    stopEngine();
}

bool USIEngine::startEngine(const QString& enginePath) {
    if (engineProcess_) {
        stopEngine();
    }
    
    engineProcess_ = new QProcess(this);
    
    connect(engineProcess_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &USIEngine::onEngineFinished);
    connect(engineProcess_, &QProcess::errorOccurred, this, &USIEngine::onEngineError);
    connect(engineProcess_, &QProcess::readyReadStandardOutput, this, &USIEngine::onEngineOutput);
    
    engineProcess_->start(enginePath);
    
    if (!engineProcess_->waitForStarted(3000)) {
        delete engineProcess_;
        engineProcess_ = nullptr;
        return false;
    }
    
    // Send initial USI command
    sendCommand("usi");
    
    return true;
}

void USIEngine::stopEngine() {
    if (engineProcess_) {
        sendCommand("quit");
        engineProcess_->waitForFinished(3000);
        engineProcess_->kill();
        engineProcess_->deleteLater();
        engineProcess_ = nullptr;
    }
    isReady_ = false;
}

bool USIEngine::isRunning() const {
    return engineProcess_ && engineProcess_->state() == QProcess::Running;
}

void USIEngine::sendCommand(const QString& command) {
    if (!engineProcess_ || !isRunning()) {
        return;
    }
    
    QTextStream stream(engineProcess_);
    stream << command << Qt::endl;
    stream.flush();
    
    pendingCommand_ = command;
    resetTimeout();
    
    qDebug() << "Sent to engine:" << command;
}

void USIEngine::initialize() {
    sendCommand("usi");
}

void USIEngine::newGame() {
    sendCommand("usinewgame");
}

void USIEngine::setPosition(const shogi::core::Board& board) {
    QString sfen = QString::fromStdString(board.toSFEN());
    sendCommand("position sfen " + sfen);
}

void USIEngine::go(int timeMs) {
    sendCommand(QString("go byoyomi %1").arg(timeMs));
}

void USIEngine::onEngineFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    
    engineProcess_->deleteLater();
    engineProcess_ = nullptr;
    isReady_ = false;
    
    emit errorOccurred("Engine process finished unexpectedly");
}

void USIEngine::onEngineError(QProcess::ProcessError error) {
    QString errorString;
    switch (error) {
        case QProcess::FailedToStart:
            errorString = "Failed to start engine";
            break;
        case QProcess::Crashed:
            errorString = "Engine crashed";
            break;
        case QProcess::Timedout:
            errorString = "Engine timed out";
            break;
        default:
            errorString = "Unknown engine error";
            break;
    }
    
    emit errorOccurred(errorString);
}

void USIEngine::onEngineOutput() {
    if (!engineProcess_) {
        return;
    }
    
    QByteArray data = engineProcess_->readAllStandardOutput();
    QStringList lines = QString::fromUtf8(data).split('\n', Qt::SkipEmptyParts);
    
    for (const QString& line : lines) {
        processUSIResponse(line.trimmed());
    }
}

void USIEngine::onCommandTimeout() {
    emit errorOccurred("Command timeout: " + pendingCommand_);
}

void USIEngine::processUSIResponse(const QString& line) {
    qDebug() << "Engine response:" << line;
    
    if (line.startsWith("id name")) {
        engineName_ = line.mid(8).trimmed();
    } else if (line.startsWith("id author")) {
        engineAuthor_ = line.mid(10).trimmed();
    } else if (line == "usiok") {
        isReady_ = true;
        emit engineReady();
    } else if (line.startsWith("bestmove")) {
        QStringList parts = line.split(' ');
        if (parts.size() >= 2) {
            emit bestMoveReceived(parts[1]);
        }
    } else if (line.startsWith("info")) {
        emit infoReceived(line);
    }
    
    commandTimer_->stop();
}

void USIEngine::resetTimeout() {
    commandTimer_->start();
}

} // namespace shogi::engine

#include "usi_engine.moc"