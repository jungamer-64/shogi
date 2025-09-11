#pragma once

#include "core/board/board.h"
#include "core/board/move.h"
#include <memory>
#include <random>

namespace shogi::ai {

/**
 * @brief Basic AI player using minimax algorithm
 */
class BasicAI {
public:
    BasicAI();
    
    // AI settings
    void setDepth(int depth) { depth_ = depth; }
    void setPlayer(shogi::core::Player player) { player_ = player; }
    
    // Main AI interface
    shogi::core::Move selectMove(const shogi::core::Board& board);
    
    // Evaluation
    int evaluatePosition(const shogi::core::Board& board, shogi::core::Player player) const;

private:
    // Minimax algorithm
    int minimax(shogi::core::Board& board, int depth, bool isMaximizing, int alpha, int beta);
    
    // Position evaluation helpers
    int getPieceValue(shogi::core::PieceType type) const;
    int getPositionalBonus(shogi::core::Position pos, shogi::core::PieceType type, shogi::core::Player player) const;
    int evaluateMobility(const shogi::core::Board& board, shogi::core::Player player) const;
    int evaluateKingSafety(const shogi::core::Board& board, shogi::core::Player player) const;
    
    int depth_;
    shogi::core::Player player_;
    std::mt19937 randomEngine_;
};

} // namespace shogi::ai