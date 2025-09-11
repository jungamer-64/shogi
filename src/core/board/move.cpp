#include "move.h"
#include "piece.h"
#include <sstream>

namespace shogi::core {

std::string Position::toString() const {
    if (!isValid()) {
        return "invalid";
    }
    return std::to_string(file) + char('a' + rank - 1);
}

Move::Move(Position from, Position to, Type type) 
    : from_(from), to_(to), type_(type) {}

Move Move::createDrop(Position to, PieceType pieceType) {
    Move move;
    move.to_ = to;
    move.type_ = Type::Drop;
    move.dropPieceType_ = pieceType;
    return move;
}

std::string Move::toString() const {
    std::ostringstream ss;
    
    if (isDrop()) {
        ss << "Drop " << static_cast<int>(dropPieceType_) << " to " << to_.toString();
    } else {
        ss << from_.toString() << "-" << to_.toString();
        if (isPromotion()) {
            ss << "+";
        }
    }
    
    return ss.str();
}

std::string Move::toUSI() const {
    std::ostringstream ss;
    
    if (isDrop()) {
        // USI format for drop: piece*square (e.g., P*5e)
        char pieceChar = 'P';  // Simplified for now
        ss << pieceChar << "*" << char('1' + 9 - to_.file) << char('a' + to_.rank - 1);
    } else {
        // USI format for move: from_square to_square [promotion]
        ss << char('1' + 9 - from_.file) << char('a' + from_.rank - 1);
        ss << char('1' + 9 - to_.file) << char('a' + to_.rank - 1);
        if (isPromotion()) {
            ss << "+";
        }
    }
    
    return ss.str();
}

bool Move::operator==(const Move& other) const {
    return from_ == other.from_ && 
           to_ == other.to_ && 
           type_ == other.type_ && 
           dropPieceType_ == other.dropPieceType_;
}

bool Move::operator!=(const Move& other) const {
    return !(*this == other);
}

} // namespace shogi::core