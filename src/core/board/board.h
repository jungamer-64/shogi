#pragma once

#include "piece.h"
#include "move.h"
#include <array>
#include <vector>
#include <memory>
#include <string>

namespace shogi::core {

/**
 * @brief Represents the Shogi board state
 */
class Board {
public:
    static constexpr int BOARD_SIZE = 9;
    using BoardArray = std::array<std::array<Piece, BOARD_SIZE>, BOARD_SIZE>;
    using Hand = std::array<int, 7>;  // Count of each piece type in hand
    
    Board();
    
    // Board access
    const Piece& getPiece(Position pos) const;
    void setPiece(Position pos, const Piece& piece);
    
    // Hand access
    const Hand& getHand(Player player) const;
    void addToHand(Player player, PieceType pieceType);
    void removeFromHand(Player player, PieceType pieceType);
    
    // Game state
    Player getCurrentPlayer() const { return currentPlayer_; }
    void setCurrentPlayer(Player player) { currentPlayer_ = player; }
    void switchPlayer();
    
    // Move operations
    bool makeMove(const Move& move);
    void undoMove();
    
    // Legal move generation
    std::vector<Move> generateLegalMoves() const;
    bool isInCheck(Player player) const;
    bool isCheckmate(Player player) const;
    
    // SFEN (Shogi Forsyth-Edwards Notation) support
    std::string toSFEN() const;
    bool fromSFEN(const std::string& sfen);
    
    // Initial setup
    void setupInitialPosition();
    void clear();
    
    // Debug
    std::string toString() const;

private:
    BoardArray board_;
    Hand senteHand_;
    Hand goteHand_;
    Player currentPlayer_;
    
    // Move history for undo functionality
    struct MoveRecord {
        Move move;
        Piece capturedPiece;
    };
    std::vector<MoveRecord> moveHistory_;
    
    // Helper methods
    bool isPositionValid(Position pos) const;
    std::vector<Move> generateMovesForPiece(Position pos) const;
    std::vector<Move> generateDropMoves() const;
    bool wouldBeInCheck(const Move& move, Player player) const;
};

} // namespace shogi::core