#include "board_widget.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>

namespace shogi::ui {

BoardWidget::BoardWidget(QWidget* parent) 
    : QWidget(parent), interactive_(true), flipped_(false), showCoordinates_(true), hasSelection_(false) {
    
    setMinimumSize(SQUARE_SIZE * 9 + BOARD_MARGIN * 2, SQUARE_SIZE * 9 + BOARD_MARGIN * 2);
    setFocusPolicy(Qt::StrongFocus);
}

void BoardWidget::setBoard(const shogi::core::Board& board) {
    board_ = board;
    update();
}

void BoardWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Fill background
    painter.fillRect(rect(), QColor(240, 217, 181));
    
    drawBoard(painter);
    if (showCoordinates_) {
        drawCoordinates(painter);
    }
    drawHighlights(painter);
    drawPieces(painter);
}

void BoardWidget::mousePressEvent(QMouseEvent* event) {
    if (!interactive_) {
        return;
    }
    
    shogi::core::Position pos = getPositionFromPoint(event->pos());
    
    if (!pos.isValid()) {
        return;
    }
    
    emit squareClicked(pos);
    
    if (hasSelection_ && selectedSquare_ != pos) {
        // Attempt to make a move
        shogi::core::Move move(selectedSquare_, pos);
        emit moveAttempted(move);
        hasSelection_ = false;
    } else {
        // Select new square
        selectedSquare_ = pos;
        hasSelection_ = true;
    }
    
    update();
}

QSize BoardWidget::sizeHint() const {
    return QSize(SQUARE_SIZE * 9 + BOARD_MARGIN * 2, SQUARE_SIZE * 9 + BOARD_MARGIN * 2);
}

void BoardWidget::drawBoard(QPainter& painter) {
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QColor(255, 235, 205));
    
    // Draw board squares
    for (int rank = 1; rank <= 9; ++rank) {
        for (int file = 1; file <= 9; ++file) {
            QRect squareRect = getSquareRect(shogi::core::Position(file, rank));
            painter.drawRect(squareRect);
        }
    }
}

void BoardWidget::drawPieces(QPainter& painter) {
    QFont font = getPieceFont();
    painter.setFont(font);
    
    for (int rank = 1; rank <= 9; ++rank) {
        for (int file = 1; file <= 9; ++file) {
            shogi::core::Position pos(file, rank);
            shogi::core::Piece piece = board_.getPiece(pos);
            
            if (!piece.isEmpty()) {
                QString pieceText = getPieceText(piece);
                QRect squareRect = getSquareRect(pos);
                
                // Set color based on player
                if (piece.getPlayer() == shogi::core::Player::Sente) {
                    painter.setPen(Qt::black);
                } else {
                    painter.setPen(Qt::red);
                }
                
                painter.drawText(squareRect, Qt::AlignCenter, pieceText);
            }
        }
    }
}

void BoardWidget::drawCoordinates(QPainter& painter) {
    painter.setPen(Qt::black);
    QFont font("Arial", 10);
    painter.setFont(font);
    
    // Draw file numbers (1-9)
    for (int file = 1; file <= 9; ++file) {
        QRect squareRect = getSquareRect(shogi::core::Position(file, 1));
        QRect coordRect(squareRect.x(), 5, squareRect.width(), 20);
        
        int displayFile = flipped_ ? file : (10 - file);
        painter.drawText(coordRect, Qt::AlignCenter, QString::number(displayFile));
    }
    
    // Draw rank letters (a-i)
    for (int rank = 1; rank <= 9; ++rank) {
        QRect squareRect = getSquareRect(shogi::core::Position(1, rank));
        QRect coordRect(5, squareRect.y(), 20, squareRect.height());
        
        char displayRank = flipped_ ? ('a' + 9 - rank) : ('a' + rank - 1);
        painter.drawText(coordRect, Qt::AlignCenter, QString(displayRank));
    }
}

void BoardWidget::drawHighlights(QPainter& painter) {
    if (hasSelection_) {
        QRect squareRect = getSquareRect(selectedSquare_);
        painter.setPen(QPen(Qt::blue, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(squareRect);
    }
}

QRect BoardWidget::getSquareRect(shogi::core::Position pos) const {
    int file = flipped_ ? pos.file : (10 - pos.file);
    int rank = flipped_ ? (10 - pos.rank) : pos.rank;
    
    int x = BOARD_MARGIN + (file - 1) * SQUARE_SIZE;
    int y = BOARD_MARGIN + (rank - 1) * SQUARE_SIZE;
    
    return QRect(x, y, SQUARE_SIZE, SQUARE_SIZE);
}

shogi::core::Position BoardWidget::getPositionFromPoint(const QPoint& point) const {
    int x = point.x() - BOARD_MARGIN;
    int y = point.y() - BOARD_MARGIN;
    
    if (x < 0 || y < 0 || x >= SQUARE_SIZE * 9 || y >= SQUARE_SIZE * 9) {
        return shogi::core::Position();  // Invalid position
    }
    
    int file = x / SQUARE_SIZE + 1;
    int rank = y / SQUARE_SIZE + 1;
    
    if (flipped_) {
        file = file;
        rank = 10 - rank;
    } else {
        file = 10 - file;
        rank = rank;
    }
    
    return shogi::core::Position(file, rank);
}

QString BoardWidget::getPieceText(const shogi::core::Piece& piece) const {
    // Use Japanese characters for pieces
    switch (piece.getType()) {
        case shogi::core::PieceType::Pawn:           return "歩";
        case shogi::core::PieceType::Lance:          return "香";
        case shogi::core::PieceType::Knight:         return "桂";
        case shogi::core::PieceType::Silver:         return "銀";
        case shogi::core::PieceType::Gold:           return "金";
        case shogi::core::PieceType::Bishop:         return "角";
        case shogi::core::PieceType::Rook:           return "飛";
        case shogi::core::PieceType::King:           return piece.getPlayer() == shogi::core::Player::Sente ? "王" : "玉";
        case shogi::core::PieceType::PromotedPawn:   return "と";
        case shogi::core::PieceType::PromotedLance:  return "杏";
        case shogi::core::PieceType::PromotedKnight: return "圭";
        case shogi::core::PieceType::PromotedSilver: return "全";
        case shogi::core::PieceType::PromotedBishop: return "馬";
        case shogi::core::PieceType::PromotedRook:   return "龍";
        default: return "?";
    }
}

QFont BoardWidget::getPieceFont() const {
    QFont font("Arial", 16, QFont::Bold);
    return font;
}

} // namespace shogi::ui

#include "board_widget.moc"