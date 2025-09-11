#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "board_widget.h"
#include "core/board/board.h"

class QTimer;

namespace shogi::ui {

/**
 * @brief Main window for the Shogi game
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    
private slots:
    void onNewGame();
    void onOpenGame();
    void onSaveGame();
    void onExit();
    void onAbout();
    
    void onSquareClicked(shogi::core::Position position);
    void onMoveAttempted(shogi::core::Move move);
    
    void updateGameInfo();

private:
    void setupUI();
    void setupMenus();
    void makeMove(const shogi::core::Move& move);
    void updateBoard();
    void updateStatus();
    
    // UI components
    BoardWidget* boardWidget_;
    QLabel* currentPlayerLabel_;
    QLabel* gameStatusLabel_;
    QTextEdit* moveHistoryText_;
    QPushButton* newGameButton_;
    QPushButton* undoButton_;
    
    // Game state
    shogi::core::Board gameBoard_;
    std::vector<shogi::core::Move> moveHistory_;
    
    // Timer for game clock (simplified)
    QTimer* gameTimer_;
};

} // namespace shogi::ui