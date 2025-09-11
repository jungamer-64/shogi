#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <expected>
#include <string>
#include <format>

// C++23 features
using std::expected;
using std::unexpected;

// Board dimensions
constexpr int BOARD_ROWS = 9;
constexpr int BOARD_COLS = 9;
constexpr int BOARD_SIZE = BOARD_ROWS * BOARD_COLS;

// Piece types
enum class PieceType : uint8_t {
    None = 0,
    King,      // 玉/王
    Rook,      // 飛車
    Bishop,    // 角行
    Gold,      // 金将
    Silver,    // 銀将
    Knight,    // 桂馬
    Lance,     // 香車
    Pawn,      // 歩兵
    // Promoted pieces
    PromotedRook,   // 竜王
    PromotedBishop, // 竜馬
    PromotedSilver, // 成銀
    PromotedKnight, // 成桂
    PromotedLance,  // 成香
    PromotedPawn,   // と金
};

// Player colors
enum class Player : uint8_t {
    First = 0,   // 先手 (bottom player)
    Second = 1   // 後手 (top player)
};

// Position on the board
struct Position {
    int8_t row{-1};
    int8_t col{-1};
    
    constexpr Position() = default;
    constexpr Position(int8_t r, int8_t c) : row(r), col(c) {}
    
    constexpr bool is_valid() const {
        return row >= 0 && row < BOARD_ROWS && col >= 0 && col < BOARD_COLS;
    }
    
    constexpr bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
    
    constexpr bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

// Invalid position constant
constexpr Position INVALID_POSITION{-1, -1};

// Piece representation
struct Piece {
    PieceType type{PieceType::None};
    Player owner{Player::First};
    
    constexpr Piece() = default;
    constexpr Piece(PieceType t, Player p) : type(t), owner(p) {}
    
    constexpr bool is_empty() const {
        return type == PieceType::None;
    }
    
    constexpr bool is_promoted() const {
        return static_cast<uint8_t>(type) >= static_cast<uint8_t>(PieceType::PromotedRook);
    }
    
    constexpr PieceType base_type() const {
        if (!is_promoted()) return type;
        switch (type) {
            case PieceType::PromotedRook: return PieceType::Rook;
            case PieceType::PromotedBishop: return PieceType::Bishop;
            case PieceType::PromotedSilver: return PieceType::Silver;
            case PieceType::PromotedKnight: return PieceType::Knight;
            case PieceType::PromotedLance: return PieceType::Lance;
            case PieceType::PromotedPawn: return PieceType::Pawn;
            default: return type;
        }
    }
    
    constexpr PieceType promoted_type() const {
        switch (type) {
            case PieceType::Rook: return PieceType::PromotedRook;
            case PieceType::Bishop: return PieceType::PromotedBishop;
            case PieceType::Silver: return PieceType::Silver; // Silver promotes to gold movement
            case PieceType::Knight: return PieceType::PromotedKnight;
            case PieceType::Lance: return PieceType::PromotedLance;
            case PieceType::Pawn: return PieceType::PromotedPawn;
            default: return type; // Cannot promote
        }
    }
    
    constexpr bool can_promote() const {
        return type == PieceType::Rook || type == PieceType::Bishop ||
               type == PieceType::Silver || type == PieceType::Knight ||
               type == PieceType::Lance || type == PieceType::Pawn;
    }
    
    constexpr bool operator==(const Piece& other) const {
        return type == other.type && owner == other.owner;
    }
};

// Move representation
struct Move {
    Position from{INVALID_POSITION};
    Position to{INVALID_POSITION};
    bool is_drop{false};        // True if placing a piece from hand
    bool is_promotion{false};   // True if promoting this move
    PieceType drop_piece{PieceType::None}; // Piece type when dropping
    
    constexpr Move() = default;
    constexpr Move(Position f, Position t, bool promote = false) 
        : from(f), to(t), is_promotion(promote) {}
    
    // Constructor for drop moves
    constexpr Move(Position t, PieceType piece) 
        : to(t), is_drop(true), drop_piece(piece) {}
    
    constexpr bool is_valid() const {
        if (is_drop) {
            return to.is_valid() && drop_piece != PieceType::None;
        }
        return from.is_valid() && to.is_valid();
    }
};

// Game result
enum class GameResult {
    None,
    FirstPlayerWin,
    SecondPlayerWin,
    Draw
};

// Error types for game operations
enum class GameError {
    InvalidMove,
    InvalidPosition,
    PieceNotFound,
    WrongPlayer,
    InCheck,
    GameOver,
    InvalidDrop,
    TwoPathRule,      // 二歩
    DropMateRule      // 打ち歩詰め
};

// Expected type for game operations
template<typename T>
using GameExpected = expected<T, GameError>;

// Hand (captured pieces)
using Hand = std::array<uint8_t, 7>; // Count of each piece type in hand

constexpr size_t piece_to_hand_index(PieceType type) {
    switch (type) {
        case PieceType::Rook: return 0;
        case PieceType::Bishop: return 1;
        case PieceType::Gold: return 2;
        case PieceType::Silver: return 3;
        case PieceType::Knight: return 4;
        case PieceType::Lance: return 5;
        case PieceType::Pawn: return 6;
        default: return 7; // Invalid
    }
}

// Convert other player
constexpr Player other_player(Player p) {
    return (p == Player::First) ? Player::Second : Player::First;
}

// Direction vectors for piece movement
struct Direction {
    int8_t row_delta;
    int8_t col_delta;
    
    constexpr Direction(int8_t r, int8_t c) : row_delta(r), col_delta(c) {}
};

// Standard directions
constexpr Direction UP{-1, 0};
constexpr Direction DOWN{1, 0};
constexpr Direction LEFT{0, -1};
constexpr Direction RIGHT{0, 1};
constexpr Direction UP_LEFT{-1, -1};
constexpr Direction UP_RIGHT{-1, 1};
constexpr Direction DOWN_LEFT{1, -1};
constexpr Direction DOWN_RIGHT{1, 1};

// Knight moves
constexpr Direction KNIGHT_UP_LEFT{-2, -1};
constexpr Direction KNIGHT_UP_RIGHT{-2, 1};