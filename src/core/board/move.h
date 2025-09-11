#pragma once

#include "piece.h"
#include <cstdint>
#include <string>

namespace shogi::core {

/**
 * @brief Represents a square position on the Shogi board
 */
struct Position {
    uint8_t file;  // 1-9 (1 is rightmost)
    uint8_t rank;  // 1-9 (1 is topmost)
    
    Position() : file(0), rank(0) {}
    Position(uint8_t f, uint8_t r) : file(f), rank(r) {}
    
    bool isValid() const {
        return file >= 1 && file <= 9 && rank >= 1 && rank <= 9;
    }
    
    std::string toString() const;
    
    bool operator==(const Position& other) const {
        return file == other.file && rank == other.rank;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Represents a move in Shogi
 */
class Move {
public:
    enum class Type {
        Normal,     // Regular move
        Drop,       // Drop from hand
        Promotion   // Move with promotion
    };
    
    Move() = default;
    
    // Constructor for normal move
    Move(Position from, Position to, Type type = Type::Normal);
    
    // Constructor for drop move
    static Move createDrop(Position to, PieceType pieceType);
    
    Position getFrom() const { return from_; }
    Position getTo() const { return to_; }
    Type getType() const { return type_; }
    PieceType getDropPieceType() const { return dropPieceType_; }
    
    bool isDrop() const { return type_ == Type::Drop; }
    bool isPromotion() const { return type_ == Type::Promotion; }
    
    std::string toString() const;
    std::string toUSI() const;
    
    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const;

private:
    Position from_;
    Position to_;
    Type type_ = Type::Normal;
    PieceType dropPieceType_ = PieceType::None;  // Only used for drop moves
};

} // namespace shogi::core