#pragma once

#include <cstdint>
#include <string>

namespace shogi::core {

/**
 * @brief Represents a piece type in Shogi
 */
enum class PieceType : uint8_t {
    None = 0,
    Pawn,
    Lance,
    Knight,
    Silver,
    Gold,
    Bishop,
    Rook,
    King,
    // Promoted pieces
    PromotedPawn,
    PromotedLance,
    PromotedKnight,
    PromotedSilver,
    PromotedBishop,
    PromotedRook
};

/**
 * @brief Represents a player/side
 */
enum class Player : uint8_t {
    None = 0,
    Sente,  // First player (black)
    Gote    // Second player (white)
};

/**
 * @brief Represents a piece with type and owner
 */
class Piece {
public:
    Piece() = default;
    Piece(PieceType type, Player player);
    
    PieceType getType() const { return type_; }
    Player getPlayer() const { return player_; }
    
    bool isEmpty() const { return type_ == PieceType::None; }
    bool isPromoted() const;
    bool canPromote() const;
    
    Piece promote() const;
    Piece demote() const;
    
    std::string toString() const;
    
    bool operator==(const Piece& other) const;
    bool operator!=(const Piece& other) const;

private:
    PieceType type_ = PieceType::None;
    Player player_ = Player::None;
};

} // namespace shogi::core