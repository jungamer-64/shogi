#include "board.h"
#include <sstream>
#include <algorithm>

namespace shogi::core {

Board::Board() : currentPlayer_(Player::Sente) {
    clear();
    setupInitialPosition();
}

const Piece& Board::getPiece(Position pos) const {
    if (!isPositionValid(pos)) {
        static Piece empty;
        return empty;
    }
    return board_[pos.rank - 1][pos.file - 1];
}

void Board::setPiece(Position pos, const Piece& piece) {
    if (isPositionValid(pos)) {
        board_[pos.rank - 1][pos.file - 1] = piece;
    }
}

const Board::Hand& Board::getHand(Player player) const {
    return (player == Player::Sente) ? senteHand_ : goteHand_;
}

void Board::addToHand(Player player, PieceType pieceType) {
    if (pieceType == PieceType::None || pieceType == PieceType::King) {
        return;
    }
    
    // Convert promoted pieces to their base form when captured
    PieceType baseType = pieceType;
    if (static_cast<int>(pieceType) >= static_cast<int>(PieceType::PromotedPawn)) {
        Piece temp(pieceType, player);
        baseType = temp.demote().getType();
    }
    
    Hand& hand = (player == Player::Sente) ? senteHand_ : goteHand_;
    int index = static_cast<int>(baseType) - 1;  // Adjust for 0-based indexing
    if (index >= 0 && index < 7) {
        hand[index]++;
    }
}

void Board::removeFromHand(Player player, PieceType pieceType) {
    Hand& hand = (player == Player::Sente) ? senteHand_ : goteHand_;
    int index = static_cast<int>(pieceType) - 1;
    if (index >= 0 && index < 7 && hand[index] > 0) {
        hand[index]--;
    }
}

void Board::switchPlayer() {
    currentPlayer_ = (currentPlayer_ == Player::Sente) ? Player::Gote : Player::Sente;
}

bool Board::makeMove(const Move& move) {
    if (move.isDrop()) {
        // Handle drop move
        if (getPiece(move.getTo()).isEmpty()) {
            setPiece(move.getTo(), Piece(move.getDropPieceType(), currentPlayer_));
            removeFromHand(currentPlayer_, move.getDropPieceType());
            
            MoveRecord record{move, Piece()};
            moveHistory_.push_back(record);
            switchPlayer();
            return true;
        }
    } else {
        // Handle regular move
        Piece movingPiece = getPiece(move.getFrom());
        Piece capturedPiece = getPiece(move.getTo());
        
        if (!movingPiece.isEmpty() && movingPiece.getPlayer() == currentPlayer_) {
            if (move.isPromotion() && movingPiece.canPromote()) {
                movingPiece = movingPiece.promote();
            }
            
            setPiece(move.getTo(), movingPiece);
            setPiece(move.getFrom(), Piece());
            
            if (!capturedPiece.isEmpty()) {
                addToHand(currentPlayer_, capturedPiece.getType());
            }
            
            MoveRecord record{move, capturedPiece};
            moveHistory_.push_back(record);
            switchPlayer();
            return true;
        }
    }
    
    return false;
}

void Board::undoMove() {
    if (moveHistory_.empty()) {
        return;
    }
    
    MoveRecord lastMove = moveHistory_.back();
    moveHistory_.pop_back();
    
    switchPlayer();  // Switch back to the player who made the move
    
    if (lastMove.move.isDrop()) {
        // Undo drop
        setPiece(lastMove.move.getTo(), Piece());
        addToHand(currentPlayer_, lastMove.move.getDropPieceType());
    } else {
        // Undo regular move
        Piece movingPiece = getPiece(lastMove.move.getTo());
        
        if (lastMove.move.isPromotion()) {
            movingPiece = movingPiece.demote();
        }
        
        setPiece(lastMove.move.getFrom(), movingPiece);
        setPiece(lastMove.move.getTo(), lastMove.capturedPiece);
        
        if (!lastMove.capturedPiece.isEmpty()) {
            removeFromHand(currentPlayer_, lastMove.capturedPiece.getType());
        }
    }
}

std::vector<Move> Board::generateLegalMoves() const {
    std::vector<Move> moves;
    
    // Generate piece moves
    for (int rank = 1; rank <= BOARD_SIZE; ++rank) {
        for (int file = 1; file <= BOARD_SIZE; ++file) {
            Position pos(file, rank);
            Piece piece = getPiece(pos);
            if (!piece.isEmpty() && piece.getPlayer() == currentPlayer_) {
                auto pieceMoves = generateMovesForPiece(pos);
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    
    // Generate drop moves
    auto dropMoves = generateDropMoves();
    moves.insert(moves.end(), dropMoves.begin(), dropMoves.end());
    
    // Filter out moves that would leave the king in check
    moves.erase(std::remove_if(moves.begin(), moves.end(),
        [this](const Move& move) {
            return wouldBeInCheck(move, currentPlayer_);
        }), moves.end());
    
    return moves;
}

bool Board::isInCheck(Player player) const {
    (void)player;
    // Simplified check detection - just return false for now
    // TODO: Implement proper check detection
    return false;
}

bool Board::isCheckmate(Player player) const {
    if (!isInCheck(player)) {
        return false;
    }
    
    // If in check and no legal moves, it's checkmate
    return generateLegalMoves().empty();
}

std::string Board::toSFEN() const {
    // Simplified SFEN implementation
    return "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1";
}

bool Board::fromSFEN(const std::string& sfen) {
    (void)sfen;
    // TODO: Implement SFEN parsing
    setupInitialPosition();
    return true;
}

void Board::setupInitialPosition() {
    clear();
    
    // Set up initial Shogi position
    // Back rank for Gote (rank 1)
    setPiece(Position(1, 1), Piece(PieceType::Lance, Player::Gote));
    setPiece(Position(2, 1), Piece(PieceType::Knight, Player::Gote));
    setPiece(Position(3, 1), Piece(PieceType::Silver, Player::Gote));
    setPiece(Position(4, 1), Piece(PieceType::Gold, Player::Gote));
    setPiece(Position(5, 1), Piece(PieceType::King, Player::Gote));
    setPiece(Position(6, 1), Piece(PieceType::Gold, Player::Gote));
    setPiece(Position(7, 1), Piece(PieceType::Silver, Player::Gote));
    setPiece(Position(8, 1), Piece(PieceType::Knight, Player::Gote));
    setPiece(Position(9, 1), Piece(PieceType::Lance, Player::Gote));
    
    // Major pieces for Gote
    setPiece(Position(2, 2), Piece(PieceType::Rook, Player::Gote));
    setPiece(Position(8, 2), Piece(PieceType::Bishop, Player::Gote));
    
    // Pawns for Gote
    for (int file = 1; file <= 9; ++file) {
        setPiece(Position(file, 3), Piece(PieceType::Pawn, Player::Gote));
    }
    
    // Pawns for Sente
    for (int file = 1; file <= 9; ++file) {
        setPiece(Position(file, 7), Piece(PieceType::Pawn, Player::Sente));
    }
    
    // Major pieces for Sente
    setPiece(Position(8, 8), Piece(PieceType::Rook, Player::Sente));
    setPiece(Position(2, 8), Piece(PieceType::Bishop, Player::Sente));
    
    // Back rank for Sente (rank 9)
    setPiece(Position(1, 9), Piece(PieceType::Lance, Player::Sente));
    setPiece(Position(2, 9), Piece(PieceType::Knight, Player::Sente));
    setPiece(Position(3, 9), Piece(PieceType::Silver, Player::Sente));
    setPiece(Position(4, 9), Piece(PieceType::Gold, Player::Sente));
    setPiece(Position(5, 9), Piece(PieceType::King, Player::Sente));
    setPiece(Position(6, 9), Piece(PieceType::Gold, Player::Sente));
    setPiece(Position(7, 9), Piece(PieceType::Silver, Player::Sente));
    setPiece(Position(8, 9), Piece(PieceType::Knight, Player::Sente));
    setPiece(Position(9, 9), Piece(PieceType::Lance, Player::Sente));
    
    currentPlayer_ = Player::Sente;
}

void Board::clear() {
    for (auto& row : board_) {
        for (auto& piece : row) {
            piece = Piece();
        }
    }
    
    senteHand_.fill(0);
    goteHand_.fill(0);
    moveHistory_.clear();
}

std::string Board::toString() const {
    std::ostringstream ss;
    
    ss << "  9 8 7 6 5 4 3 2 1\n";
    for (int rank = 1; rank <= BOARD_SIZE; ++rank) {
        ss << rank << " ";
        for (int file = BOARD_SIZE; file >= 1; --file) {
            Piece piece = getPiece(Position(file, rank));
            ss << piece.toString() << " ";
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool Board::isPositionValid(Position pos) const {
    return pos.isValid();
}

std::vector<Move> Board::generateMovesForPiece(Position pos) const {
    (void)pos;
    std::vector<Move> moves;
    // TODO: Implement piece-specific move generation
    return moves;
}

std::vector<Move> Board::generateDropMoves() const {
    std::vector<Move> moves;
    // TODO: Implement drop move generation
    return moves;
}

bool Board::wouldBeInCheck(const Move& move, Player player) const {
    (void)move;
    (void)player;
    // TODO: Implement check detection after move
    return false;
}

} // namespace shogi::core