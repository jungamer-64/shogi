#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>
#include "core/board/board.h"

namespace shogi::engine {

/**
 * @brief USI (Universal Shogi Interface) engine wrapper
 */
class USIEngine : public QObject {
    Q_OBJECT

public:
    explicit USIEngine(QObject* parent = nullptr);
    ~USIEngine();
    
    // Engine management
    bool startEngine(const QString& enginePath);
    void stopEngine();
    bool isRunning() const;
    
    // USI protocol
    void sendCommand(const QString& command);
    void initialize();
    void newGame();
    void setPosition(const shogi::core::Board& board);
    void go(int timeMs = 1000);
    
    // Engine info
    QString getEngineName() const { return engineName_; }
    QString getEngineAuthor() const { return engineAuthor_; }

signals:
    void engineReady();
    void bestMoveReceived(const QString& move);
    void infoReceived(const QString& info);
    void errorOccurred(const QString& error);

private slots:
    void onEngineFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onEngineError(QProcess::ProcessError error);
    void onEngineOutput();
    void onCommandTimeout();

private:
    void processUSIResponse(const QString& line);
    void resetTimeout();
    
    QProcess* engineProcess_;
    QTimer* commandTimer_;
    QString engineName_;
    QString engineAuthor_;
    bool isReady_;
    QString pendingCommand_;
};

} // namespace shogi::engine