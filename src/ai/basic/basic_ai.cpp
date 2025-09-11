#include "basic_ai.h"
#include <algorithm>
#include <limits>

namespace shogi::ai {

BasicAI::BasicAI() : depth_(3), player_(shogi::core::Player::Gote), randomEngine_(std::random_device{}()) {}

shogi::core::Move BasicAI::selectMove(const shogi::core::Board& board) {
    auto legalMoves = board.generateLegalMoves();
    
    if (legalMoves.empty()) {
        return shogi::core::Move();  // No legal moves
    }
    
    if (legalMoves.size() == 1) {
        return legalMoves[0];  // Only one legal move
    }
    
    // Use minimax to find best move
    shogi::core::Move bestMove;
    int bestScore = std::numeric_limits<int>::min();
    
    for (const auto& move : legalMoves) {
        shogi::core::Board tempBoard = board;
        tempBoard.makeMove(move);
        
        int score = minimax(tempBoard, depth_ - 1, false, 
                           std::numeric_limits<int>::min(), 
                           std::numeric_limits<int>::max());
        
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    
    return bestMove;
}

int BasicAI::evaluatePosition(const shogi::core::Board& board, shogi::core::Player player) const {
    int score = 0;
    
    // Material evaluation
    for (int rank = 1; rank <= 9; ++rank) {
        for (int file = 1; file <= 9; ++file) {
            shogi::core::Position pos(file, rank);
            shogi::core::Piece piece = board.getPiece(pos);
            
            if (!piece.isEmpty()) {
                int pieceValue = getPieceValue(piece.getType());
                int positionalBonus = getPositionalBonus(pos, piece.getType(), piece.getPlayer());
                
                if (piece.getPlayer() == player) {
                    score += pieceValue + positionalBonus;
                } else {
                    score -= pieceValue + positionalBonus;
                }
            }
        }
    }
    
    // Hand evaluation
    const auto& hand = board.getHand(player);
    const auto& opponentHand = board.getHand(
        player == shogi::core::Player::Sente ? shogi::core::Player::Gote : shogi::core::Player::Sente);
    
    for (int i = 0; i < 7; ++i) {
        shogi::core::PieceType type = static_cast<shogi::core::PieceType>(i + 1);
        score += hand[i] * getPieceValue(type);
        score -= opponentHand[i] * getPieceValue(type);
    }
    
    // Add positional factors
    score += evaluateMobility(board, player);
    score += evaluateKingSafety(board, player);
    
    return score;
}

int BasicAI::minimax(shogi::core::Board& board, int depth, bool isMaximizing, int alpha, int beta) {
    if (depth == 0 || board.isCheckmate(board.getCurrentPlayer())) {
        return evaluatePosition(board, player_);
    }
    
    auto legalMoves = board.generateLegalMoves();
    
    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : legalMoves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, false, alpha, beta);
            board.undoMove();
            
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            
            if (beta <= alpha) {
                break;  // Alpha-beta pruning
            }
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : legalMoves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, true, alpha, beta);
            board.undoMove();
            
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            
            if (beta <= alpha) {
                break;  // Alpha-beta pruning
            }
        }
        return minEval;
    }
}

int BasicAI::getPieceValue(shogi::core::PieceType type) const {
    switch (type) {
        case shogi::core::PieceType::Pawn:           return 100;
        case shogi::core::PieceType::Lance:          return 300;
        case shogi::core::PieceType::Knight:         return 350;
        case shogi::core::PieceType::Silver:         return 400;
        case shogi::core::PieceType::Gold:           return 500;
        case shogi::core::PieceType::Bishop:         return 800;
        case shogi::core::PieceType::Rook:           return 1000;
        case shogi::core::PieceType::King:           return 10000;
        case shogi::core::PieceType::PromotedPawn:   return 600;
        case shogi::core::PieceType::PromotedLance:  return 600;
        case shogi::core::PieceType::PromotedKnight: return 600;
        case shogi::core::PieceType::PromotedSilver: return 600;
        case shogi::core::PieceType::PromotedBishop: return 1200;
        case shogi::core::PieceType::PromotedRook:   return 1400;
        default: return 0;
    }
}

int BasicAI::getPositionalBonus(shogi::core::Position pos, shogi::core::PieceType type, shogi::core::Player player) const {
    (void)pos;
    (void)type;
    (void)player;
    
    // Simplified - just return 0 for now
    // TODO: Implement piece-specific positional bonuses
    return 0;
}

int BasicAI::evaluateMobility(const shogi::core::Board& board, shogi::core::Player player) const {
    (void)board;
    (void)player;
    
    // Simplified - just return 0 for now
    // TODO: Count legal moves for mobility evaluation
    return 0;
}

int BasicAI::evaluateKingSafety(const shogi::core::Board& board, shogi::core::Player player) const {
    (void)board;
    (void)player;
    
    // Simplified - just return 0 for now
    // TODO: Evaluate king safety based on surrounding pieces
    return 0;
}

} // namespace shogi::ai