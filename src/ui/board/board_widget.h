#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include "core/board/board.h"

namespace shogi::ui {

/**
 * @brief Qt widget for displaying and interacting with the Shogi board
 */
class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr);
    
    // Board state
    void setBoard(const shogi::core::Board& board);
    const shogi::core::Board& getBoard() const { return board_; }
    
    // Interaction
    void setInteractive(bool interactive) { interactive_ = interactive; }
    bool isInteractive() const { return interactive_; }
    
    // Visual settings
    void setFlipped(bool flipped) { flipped_ = flipped; update(); }
    bool isFlipped() const { return flipped_; }
    
    void setShowCoordinates(bool show) { showCoordinates_ = show; update(); }
    bool showCoordinates() const { return showCoordinates_; }

signals:
    void squareClicked(shogi::core::Position position);
    void moveAttempted(shogi::core::Move move);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    QSize sizeHint() const override;

private:
    void drawBoard(QPainter& painter);
    void drawPieces(QPainter& painter);
    void drawCoordinates(QPainter& painter);
    void drawHighlights(QPainter& painter);
    
    QRect getSquareRect(shogi::core::Position pos) const;
    shogi::core::Position getPositionFromPoint(const QPoint& point) const;
    
    QString getPieceText(const shogi::core::Piece& piece) const;
    QFont getPieceFont() const;
    
    shogi::core::Board board_;
    bool interactive_;
    bool flipped_;
    bool showCoordinates_;
    
    // Interaction state
    shogi::core::Position selectedSquare_;
    bool hasSelection_;
    
    // Visual constants
    static constexpr int SQUARE_SIZE = 60;
    static constexpr int BOARD_MARGIN = 30;
};

} // namespace shogi::ui