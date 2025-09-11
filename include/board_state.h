#pragma once

#include "common.h"
#include <array>

class BoardState {
public:
    BoardState();
    
    // Initialize to starting position
    void setup_initial_position();
    
    // Board access
    const Piece& get_piece(Position pos) const;
    void set_piece(Position pos, const Piece& piece);
    void clear_piece(Position pos);
    
    // Hand access
    const Hand& get_hand(Player player) const;
    void add_to_hand(Player player, PieceType piece);
    bool remove_from_hand(Player player, PieceType piece);
    uint8_t get_hand_count(Player player, PieceType piece) const;
    
    // Board state queries
    Position find_king(Player player) const;
    bool is_square_attacked(Position pos, Player by_player) const;
    bool is_in_check(Player player) const;
    
    // Game state
    Player current_player() const { return m_current_player; }
    void set_current_player(Player player) { m_current_player = player; }
    void switch_player() { m_current_player = other_player(m_current_player); }
    
    // Board representation
    const std::array<Piece, BOARD_SIZE>& get_board() const { return m_board; }
    
    // Copy operations
    BoardState(const BoardState& other) = default;
    BoardState& operator=(const BoardState& other) = default;
    
private:
    std::array<Piece, BOARD_SIZE> m_board;
    std::array<Hand, 2> m_hands; // hands[0] = first player, hands[1] = second player
    Player m_current_player{Player::First};
    
    // Helper for position to index conversion
    size_t pos_to_index(Position pos) const;
    bool is_valid_index(size_t index) const;
};