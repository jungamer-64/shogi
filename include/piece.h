#pragma once

#include "common.h"
#include <vector>

class PieceMovement {
public:
    // Get all possible moves for a piece at given position
    static std::vector<Position> get_possible_moves(
        const Piece& piece, 
        Position from, 
        const std::array<Piece, BOARD_SIZE>& board
    );
    
    // Check if a move is valid for the given piece
    static bool is_valid_move(
        const Piece& piece,
        Position from,
        Position to,
        const std::array<Piece, BOARD_SIZE>& board
    );
    
    // Check if position is in promotion zone for given player
    static bool is_promotion_zone(Position pos, Player player);
    
    // Check if piece must promote (e.g., pawn/lance at far end)
    static bool must_promote(const Piece& piece, Position to);
    
    // Check if piece can promote
    static bool can_promote(const Piece& piece, Position from, Position to);

private:
    // Movement patterns for each piece type
    static std::vector<Position> get_king_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_rook_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_bishop_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_gold_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_silver_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_knight_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_lance_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_pawn_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_promoted_rook_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static std::vector<Position> get_promoted_bishop_moves(Position from, const std::array<Piece, BOARD_SIZE>& board, Player player);
    
    // Helper functions
    static std::vector<Position> get_sliding_moves(
        Position from, 
        const std::vector<Direction>& directions,
        const std::array<Piece, BOARD_SIZE>& board,
        Player player
    );
    
    static std::vector<Position> get_single_step_moves(
        Position from,
        const std::vector<Direction>& directions,
        const std::array<Piece, BOARD_SIZE>& board,
        Player player
    );
    
    static bool is_enemy_or_empty(Position pos, const std::array<Piece, BOARD_SIZE>& board, Player player);
    static Position apply_direction(Position pos, Direction dir, Player player);
};