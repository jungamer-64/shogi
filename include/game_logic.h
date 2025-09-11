#pragma once

#include "common.h"
#include "board_state.h"
#include <vector>
#include <functional>

class GameLogic {
public:
    GameLogic();
    
    // Game state
    const BoardState& get_board_state() const { return m_board_state; }
    GameResult get_game_result() const { return m_game_result; }
    Player get_current_player() const { return m_board_state.current_player(); }
    
    // Game management
    void reset_game();
    
    // Move validation and execution
    GameExpected<bool> make_move(const Move& move);
    std::vector<Move> get_legal_moves() const;
    std::vector<Position> get_legal_destinations(Position from) const;
    std::vector<PieceType> get_droppable_pieces(Position to) const;
    
    // Check game state
    bool is_in_check(Player player) const;
    bool is_checkmate(Player player) const;
    bool is_stalemate() const;
    
    // Move callbacks for UI updates
    using MoveCallback = std::function<void(const Move&, const BoardState&)>;
    void set_move_callback(MoveCallback callback) { m_move_callback = callback; }
    
    // Game state queries
    bool is_game_over() const { return m_game_result != GameResult::None; }
    
private:
    BoardState m_board_state;
    GameResult m_game_result{GameResult::None};
    MoveCallback m_move_callback;
    
    // Move validation helpers
    GameExpected<bool> validate_normal_move(const Move& move) const;
    GameExpected<bool> validate_drop_move(const Move& move) const;
    bool would_be_in_check_after_move(const Move& move, Player player) const;
    bool is_nifu_violation(const Move& move) const; // 二歩 check
    bool is_uchifuzume_violation(const Move& move) const; // 打ち歩詰め check
    
    // Move execution
    void execute_move(const Move& move);
    void execute_normal_move(const Move& move);
    void execute_drop_move(const Move& move);
    
    // Game end detection
    void check_game_end();
    
    // Helper functions
    std::vector<Move> generate_all_moves(Player player) const;
    BoardState simulate_move(const Move& move) const;
};