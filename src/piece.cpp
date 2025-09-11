#include "piece.h"
#include <algorithm>

// Convert board position to array index
constexpr size_t pos_to_index(Position pos) {
    return pos.row * BOARD_COLS + pos.col;
}

// Convert array index to board position
constexpr Position index_to_pos(size_t index) {
    return Position(index / BOARD_COLS, index % BOARD_COLS);
}

std::vector<Position> PieceMovement::get_possible_moves(
    const Piece& piece, 
    Position from, 
    const std::array<Piece, BOARD_SIZE>& board
) {
    if (piece.is_empty() || !from.is_valid()) {
        return {};
    }
    
    switch (piece.type) {
        case PieceType::King:
            return get_king_moves(from, board, piece.owner);
        case PieceType::Rook:
            return get_rook_moves(from, board, piece.owner);
        case PieceType::Bishop:
            return get_bishop_moves(from, board, piece.owner);
        case PieceType::Gold:
            return get_gold_moves(from, board, piece.owner);
        case PieceType::Silver:
            return get_silver_moves(from, board, piece.owner);
        case PieceType::Knight:
            return get_knight_moves(from, board, piece.owner);
        case PieceType::Lance:
            return get_lance_moves(from, board, piece.owner);
        case PieceType::Pawn:
            return get_pawn_moves(from, board, piece.owner);
        case PieceType::PromotedRook:
            return get_promoted_rook_moves(from, board, piece.owner);
        case PieceType::PromotedBishop:
            return get_promoted_bishop_moves(from, board, piece.owner);
        case PieceType::PromotedSilver:
        case PieceType::PromotedKnight:
        case PieceType::PromotedLance:
        case PieceType::PromotedPawn:
            return get_gold_moves(from, board, piece.owner); // All promote to gold movement
        default:
            return {};
    }
}

bool PieceMovement::is_valid_move(
    const Piece& piece,
    Position from,
    Position to,
    const std::array<Piece, BOARD_SIZE>& board
) {
    auto possible_moves = get_possible_moves(piece, from, board);
    return std::find(possible_moves.begin(), possible_moves.end(), to) != possible_moves.end();
}

bool PieceMovement::is_promotion_zone(Position pos, Player player) {
    if (player == Player::First) {
        return pos.row <= 2; // Top 3 rows for first player
    } else {
        return pos.row >= 6; // Bottom 3 rows for second player
    }
}

bool PieceMovement::must_promote(const Piece& piece, Position to) {
    if (piece.owner == Player::First) {
        // First player moving up the board
        if (piece.type == PieceType::Pawn || piece.type == PieceType::Lance) {
            return to.row == 0; // Must promote at top row
        }
        if (piece.type == PieceType::Knight) {
            return to.row <= 1; // Must promote at top 2 rows
        }
    } else {
        // Second player moving down the board
        if (piece.type == PieceType::Pawn || piece.type == PieceType::Lance) {
            return to.row == 8; // Must promote at bottom row
        }
        if (piece.type == PieceType::Knight) {
            return to.row >= 7; // Must promote at bottom 2 rows
        }
    }
    return false;
}

bool PieceMovement::can_promote(const Piece& piece, Position from, Position to) {
    if (!piece.can_promote() || piece.is_promoted()) {
        return false;
    }
    
    // Can promote if moving from or to promotion zone
    return is_promotion_zone(from, piece.owner) || is_promotion_zone(to, piece.owner);
}

Position PieceMovement::apply_direction(Position pos, Direction dir, Player player) {
    // For second player, flip the direction vertically
    int row_dir = (player == Player::Second) ? -dir.row_delta : dir.row_delta;
    int col_dir = dir.col_delta;
    
    return Position(pos.row + row_dir, pos.col + col_dir);
}

bool PieceMovement::is_enemy_or_empty(Position pos, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    if (!pos.is_valid()) return false;
    
    const Piece& piece = board[pos_to_index(pos)];
    return piece.is_empty() || piece.owner != player;
}

std::vector<Position> PieceMovement::get_sliding_moves(
    Position from, 
    const std::vector<Direction>& directions,
    const std::array<Piece, BOARD_SIZE>& board,
    Player player
) {
    std::vector<Position> moves;
    
    for (const auto& dir : directions) {
        Position current = apply_direction(from, dir, player);
        
        while (current.is_valid()) {
            const Piece& piece = board[pos_to_index(current)];
            
            if (piece.is_empty()) {
                moves.push_back(current);
            } else if (piece.owner != player) {
                moves.push_back(current); // Can capture
                break; // Cannot move further
            } else {
                break; // Blocked by own piece
            }
            
            current = apply_direction(current, dir, player);
        }
    }
    
    return moves;
}

std::vector<Position> PieceMovement::get_single_step_moves(
    Position from,
    const std::vector<Direction>& directions,
    const std::array<Piece, BOARD_SIZE>& board,
    Player player
) {
    std::vector<Position> moves;
    
    for (const auto& dir : directions) {
        Position to = apply_direction(from, dir, player);
        if (is_enemy_or_empty(to, board, player)) {
            moves.push_back(to);
        }
    }
    
    return moves;
}

std::vector<Position> PieceMovement::get_king_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {
        UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
    };
    return get_single_step_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_rook_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {UP, DOWN, LEFT, RIGHT};
    return get_sliding_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_bishop_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};
    return get_sliding_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_gold_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT};
    return get_single_step_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_silver_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {UP, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT};
    return get_single_step_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_knight_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {KNIGHT_UP_LEFT, KNIGHT_UP_RIGHT};
    return get_single_step_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_lance_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {UP};
    return get_sliding_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_pawn_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    std::vector<Direction> directions = {UP};
    return get_single_step_moves(from, directions, board, player);
}

std::vector<Position> PieceMovement::get_promoted_rook_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    // Promoted rook = rook + king
    auto rook_moves = get_rook_moves(from, board, player);
    auto king_moves = get_king_moves(from, board, player);
    
    // Merge and remove duplicates
    rook_moves.insert(rook_moves.end(), king_moves.begin(), king_moves.end());
    std::sort(rook_moves.begin(), rook_moves.end(), [](const Position& a, const Position& b) {
        if (a.row != b.row) return a.row < b.row;
        return a.col < b.col;
    });
    rook_moves.erase(std::unique(rook_moves.begin(), rook_moves.end()), rook_moves.end());
    
    return rook_moves;
}

std::vector<Position> PieceMovement::get_promoted_bishop_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player) {
    // Promoted bishop = bishop + king
    auto bishop_moves = get_bishop_moves(from, board, player);
    auto king_moves = get_king_moves(from, board, player);
    
    // Merge and remove duplicates
    bishop_moves.insert(bishop_moves.end(), king_moves.begin(), king_moves.end());
    std::sort(bishop_moves.begin(), bishop_moves.end(), [](const Position& a, const Position& b) {
        if (a.row != b.row) return a.row < b.row;
        return a.col < b.col;
    });
    bishop_moves.erase(std::unique(bishop_moves.begin(), bishop_moves.end()), bishop_moves.end());
    
    return bishop_moves;
}