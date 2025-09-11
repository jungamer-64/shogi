#include "main_window.h"
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

namespace shogi::ui {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), gameTimer_(new QTimer(this)) {
    setupUI();
    setupMenus();
    
    // Initialize game
    onNewGame();
    
    connect(gameTimer_, &QTimer::timeout, this, &MainWindow::updateGameInfo);
    gameTimer_->start(1000);  // Update every second
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Left side - board
    boardWidget_ = new BoardWidget(this);
    connect(boardWidget_, &BoardWidget::squareClicked, this, &MainWindow::onSquareClicked);
    connect(boardWidget_, &BoardWidget::moveAttempted, this, &MainWindow::onMoveAttempted);
    
    mainLayout->addWidget(boardWidget_, 2);
    
    // Right side - game info
    QVBoxLayout* infoLayout = new QVBoxLayout;
    
    // Game status
    currentPlayerLabel_ = new QLabel("Current Player: Sente", this);
    gameStatusLabel_ = new QLabel("Game in progress", this);
    
    infoLayout->addWidget(currentPlayerLabel_);
    infoLayout->addWidget(gameStatusLabel_);
    
    // Control buttons
    newGameButton_ = new QPushButton("New Game", this);
    undoButton_ = new QPushButton("Undo Move", this);
    
    connect(newGameButton_, &QPushButton::clicked, this, &MainWindow::onNewGame);
    connect(undoButton_, &QPushButton::clicked, [this]() {
        if (!moveHistory_.empty()) {
            gameBoard_.undoMove();
            moveHistory_.pop_back();
            updateBoard();
            updateStatus();
        }
    });
    
    infoLayout->addWidget(newGameButton_);
    infoLayout->addWidget(undoButton_);
    
    // Move history
    QLabel* historyLabel = new QLabel("Move History:", this);
    moveHistoryText_ = new QTextEdit(this);
    moveHistoryText_->setMaximumHeight(200);
    moveHistoryText_->setReadOnly(true);
    
    infoLayout->addWidget(historyLabel);
    infoLayout->addWidget(moveHistoryText_);
    
    infoLayout->addStretch();
    
    QWidget* infoWidget = new QWidget(this);
    infoWidget->setLayout(infoLayout);
    infoWidget->setMaximumWidth(300);
    
    mainLayout->addWidget(infoWidget, 0);
    
    setWindowTitle("Shogi Game");
    resize(800, 600);
}

void MainWindow::setupMenus() {
    QMenuBar* menuBar = this->menuBar();
    
    // File menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    
    QAction* newAction = fileMenu->addAction("&New Game");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewGame);
    
    QAction* openAction = fileMenu->addAction("&Open Game");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenGame);
    
    QAction* saveAction = fileMenu->addAction("&Save Game");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveGame);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
    
    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    
    statusBar()->showMessage("Ready");
}

void MainWindow::onNewGame() {
    gameBoard_.setupInitialPosition();
    moveHistory_.clear();
    updateBoard();
    updateStatus();
    moveHistoryText_->clear();
    statusBar()->showMessage("New game started");
}

void MainWindow::onOpenGame() {
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Open Game", "", "SFEN Files (*.sfen);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement SFEN loading
        statusBar()->showMessage("Loading game files not yet implemented");
    }
}

void MainWindow::onSaveGame() {
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Game", "", "SFEN Files (*.sfen);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement SFEN saving
        statusBar()->showMessage("Saving game files not yet implemented");
    }
}

void MainWindow::onExit() {
    close();
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "About Shogi Game",
        "Shogi Game v1.0\n\n"
        "A Qt6-based Shogi game with modern C++ architecture.\n\n"
        "Features:\n"
        "- Local and online play\n"
        "- Handicap games (Komaochi)\n"
        "- USI engine support\n"
        "- Built-in AI\n\n"
        "Built with Qt6 and C++17");
}

void MainWindow::onSquareClicked(shogi::core::Position position) {
    Q_UNUSED(position)
    // Square click is handled by the board widget
}

void MainWindow::onMoveAttempted(shogi::core::Move move) {
    makeMove(move);
}

void MainWindow::makeMove(const shogi::core::Move& move) {
    if (gameBoard_.makeMove(move)) {
        moveHistory_.push_back(move);
        updateBoard();
        updateStatus();
        
        // Add move to history display
        QString moveText = QString::fromStdString(move.toString());
        moveHistoryText_->append(QString("%1. %2").arg(moveHistory_.size()).arg(moveText));
        
        statusBar()->showMessage("Move made: " + moveText);
    } else {
        statusBar()->showMessage("Invalid move");
    }
}

void MainWindow::updateBoard() {
    boardWidget_->setBoard(gameBoard_);
}

void MainWindow::updateStatus() {
    // Update current player
    QString playerName = (gameBoard_.getCurrentPlayer() == shogi::core::Player::Sente) ? "Sente" : "Gote";
    currentPlayerLabel_->setText("Current Player: " + playerName);
    
    // Check for game end conditions
    if (gameBoard_.isCheckmate(gameBoard_.getCurrentPlayer())) {
        QString winner = (gameBoard_.getCurrentPlayer() == shogi::core::Player::Sente) ? "Gote" : "Sente";
        gameStatusLabel_->setText("Checkmate! " + winner + " wins!");
    } else if (gameBoard_.isInCheck(gameBoard_.getCurrentPlayer())) {
        gameStatusLabel_->setText("Check!");
    } else {
        gameStatusLabel_->setText("Game in progress");
    }
}

void MainWindow::updateGameInfo() {
    // This is called periodically to update time displays, etc.
    // For now, just ensure the status is current
    updateStatus();
}

} // namespace shogi::ui

#include "main_window.moc"