#include "game_logic.h"
#include "piece.h"
#include <algorithm>

GameLogic::GameLogic() {
    reset_game();
}

void GameLogic::reset_game() {
    m_board_state.setup_initial_position();
    m_game_result = GameResult::None;
}

GameExpected<bool> GameLogic::make_move(const Move& move) {
    if (m_game_result != GameResult::None) {
        return unexpected(GameError::GameOver);
    }
    
    // Validate the move
    GameExpected<bool> validation_result;
    if (move.is_drop) {
        validation_result = validate_drop_move(move);
    } else {
        validation_result = validate_normal_move(move);
    }
    
    if (!validation_result) {
        return validation_result;
    }
    
    // Check if this move would leave the player in check
    if (would_be_in_check_after_move(move, m_board_state.current_player())) {
        return unexpected(GameError::InCheck);
    }
    
    // Execute the move
    execute_move(move);
    
    // Switch players
    m_board_state.switch_player();
    
    // Check for game end
    check_game_end();
    
    // Notify UI
    if (m_move_callback) {
        m_move_callback(move, m_board_state);
    }
    
    return true;
}

GameExpected<bool> GameLogic::validate_normal_move(const Move& move) const {
    if (!move.from.is_valid() || !move.to.is_valid()) {
        return unexpected(GameError::InvalidPosition);
    }
    
    const Piece& piece = m_board_state.get_piece(move.from);
    if (piece.is_empty()) {
        return unexpected(GameError::PieceNotFound);
    }
    
    if (piece.owner != m_board_state.current_player()) {
        return unexpected(GameError::WrongPlayer);
    }
    
    // Check if the move is valid for this piece
    if (!PieceMovement::is_valid_move(piece, move.from, move.to, m_board_state.get_board())) {
        return unexpected(GameError::InvalidMove);
    }
    
    // Check promotion rules
    if (move.is_promotion) {
        if (!PieceMovement::can_promote(piece, move.from, move.to)) {
            return unexpected(GameError::InvalidMove);
        }
    } else {
        if (PieceMovement::must_promote(piece, move.to)) {
            return unexpected(GameError::InvalidMove);
        }
    }
    
    return true;
}

GameExpected<bool> GameLogic::validate_drop_move(const Move& move) const {
    if (!move.to.is_valid() || move.drop_piece == PieceType::None) {
        return unexpected(GameError::InvalidPosition);
    }
    
    // Check if the square is empty
    if (!m_board_state.get_piece(move.to).is_empty()) {
        return unexpected(GameError::InvalidDrop);
    }
    
    // Check if player has the piece in hand
    if (m_board_state.get_hand_count(m_board_state.current_player(), move.drop_piece) == 0) {
        return unexpected(GameError::InvalidDrop);
    }
    
    // Check for nifu (two pawns in same column)
    if (move.drop_piece == PieceType::Pawn && is_nifu_violation(move)) {
        return unexpected(GameError::TwoPathRule);
    }
    
    // Check for uchifuzume (pawn drop mate)
    if (move.drop_piece == PieceType::Pawn && is_uchifuzume_violation(move)) {
        return unexpected(GameError::DropMateRule);
    }
    
    // Check if pawn/lance/knight can move from this position
    Player current = m_board_state.current_player();
    if ((move.drop_piece == PieceType::Pawn || move.drop_piece == PieceType::Lance) &&
        ((current == Player::First && move.to.row == 0) ||
         (current == Player::Second && move.to.row == 8))) {
        return unexpected(GameError::InvalidDrop);
    }
    
    if (move.drop_piece == PieceType::Knight &&
        ((current == Player::First && move.to.row <= 1) ||
         (current == Player::Second && move.to.row >= 7))) {
        return unexpected(GameError::InvalidDrop);
    }
    
    return true;
}

bool GameLogic::would_be_in_check_after_move(const Move& move, Player player) const {
    BoardState test_state = simulate_move(move);
    return test_state.is_in_check(player);
}

bool GameLogic::is_nifu_violation(const Move& move) const {
    if (move.drop_piece != PieceType::Pawn) return false;
    
    Player current = m_board_state.current_player();
    
    // Check if there's already a pawn in this column
    for (int row = 0; row < BOARD_ROWS; ++row) {
        const Piece& piece = m_board_state.get_piece({static_cast<int8_t>(row), move.to.col});
        if (piece.type == PieceType::Pawn && piece.owner == current) {
            return true;
        }
    }
    
    return false;
}

bool GameLogic::is_uchifuzume_violation(const Move& move) const {
    if (move.drop_piece != PieceType::Pawn) return false;
    
    // Simulate the pawn drop
    BoardState test_state = simulate_move(move);
    
    // Check if this puts the opponent in checkmate
    Player opponent = other_player(m_board_state.current_player());
    if (!test_state.is_in_check(opponent)) {
        return false; // Not even check, so not mate
    }
    
    // Generate all possible moves for opponent
    std::vector<Move> opponent_moves = generate_all_moves(opponent);
    
    // Check if any move gets out of check
    for (const auto& escape_move : opponent_moves) {
        // Simulate opponent's move (simplified)
        if (!would_be_in_check_after_move(escape_move, opponent)) {
            return false; // Found an escape, not mate
        }
    }
    
    return true; // It's mate by pawn drop
}

void GameLogic::execute_move(const Move& move) {
    if (move.is_drop) {
        execute_drop_move(move);
    } else {
        execute_normal_move(move);
    }
}

void GameLogic::execute_normal_move(const Move& move) {
    Piece piece = m_board_state.get_piece(move.from);
    const Piece& target = m_board_state.get_piece(move.to);
    
    // Capture piece if present
    if (!target.is_empty()) {
        m_board_state.add_to_hand(piece.owner, target.type);
    }
    
    // Handle promotion
    if (move.is_promotion && piece.can_promote()) {
        piece.type = piece.promoted_type();
    }
    
    // Move the piece
    m_board_state.clear_piece(move.from);
    m_board_state.set_piece(move.to, piece);
}

void GameLogic::execute_drop_move(const Move& move) {
    Piece piece{move.drop_piece, m_board_state.current_player()};
    
    // Remove from hand
    m_board_state.remove_from_hand(m_board_state.current_player(), move.drop_piece);
    
    // Place on board
    m_board_state.set_piece(move.to, piece);
}

std::vector<Move> GameLogic::get_legal_moves() const {
    return generate_all_moves(m_board_state.current_player());
}

std::vector<Position> GameLogic::get_legal_destinations(Position from) const {
    const Piece& piece = m_board_state.get_piece(from);
    if (piece.is_empty() || piece.owner != m_board_state.current_player()) {
        return {};
    }
    
    auto possible_moves = PieceMovement::get_possible_moves(piece, from, m_board_state.get_board());
    std::vector<Position> legal_destinations;
    
    for (const auto& to : possible_moves) {
        Move test_move{from, to};
        if (validate_normal_move(test_move) && 
            !would_be_in_check_after_move(test_move, m_board_state.current_player())) {
            legal_destinations.push_back(to);
        }
    }
    
    return legal_destinations;
}

std::vector<PieceType> GameLogic::get_droppable_pieces(Position to) const {
    std::vector<PieceType> droppable;
    
    if (!m_board_state.get_piece(to).is_empty()) {
        return droppable; // Square is occupied
    }
    
    const auto& hand = m_board_state.get_hand(m_board_state.current_player());
    const std::array<PieceType, 7> piece_types = {
        PieceType::Rook, PieceType::Bishop, PieceType::Gold,
        PieceType::Silver, PieceType::Knight, PieceType::Lance, PieceType::Pawn
    };
    
    for (size_t i = 0; i < piece_types.size(); ++i) {
        if (hand[i] > 0) {
            Move test_move{to, piece_types[i]};
            if (validate_drop_move(test_move) &&
                !would_be_in_check_after_move(test_move, m_board_state.current_player())) {
                droppable.push_back(piece_types[i]);
            }
        }
    }
    
    return droppable;
}

bool GameLogic::is_in_check(Player player) const {
    return m_board_state.is_in_check(player);
}

bool GameLogic::is_checkmate(Player player) const {
    if (!is_in_check(player)) {
        return false; // Not in check, so not mate
    }
    
    // Generate all possible moves and see if any gets out of check
    auto moves = generate_all_moves(player);
    for (const auto& move : moves) {
        if (!would_be_in_check_after_move(move, player)) {
            return false; // Found an escape
        }
    }
    
    return true; // No escape found
}

bool GameLogic::is_stalemate() const {
    Player current = m_board_state.current_player();
    if (is_in_check(current)) {
        return false; // In check, so not stalemate
    }
    
    auto moves = generate_all_moves(current);
    return moves.empty(); // No legal moves and not in check
}

void GameLogic::check_game_end() {
    Player current = m_board_state.current_player();
    
    if (is_checkmate(current)) {
        m_game_result = (current == Player::First) ? GameResult::SecondPlayerWin : GameResult::FirstPlayerWin;
    } else if (is_stalemate()) {
        m_game_result = GameResult::Draw;
    }
}

std::vector<Move> GameLogic::generate_all_moves(Player player) const {
    std::vector<Move> moves;
    
    // Generate normal moves
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            Position from{static_cast<int8_t>(row), static_cast<int8_t>(col)};
            const Piece& piece = m_board_state.get_piece(from);
            
            if (piece.is_empty() || piece.owner != player) {
                continue;
            }
            
            auto destinations = PieceMovement::get_possible_moves(piece, from, m_board_state.get_board());
            for (const auto& to : destinations) {
                Move normal_move{from, to, false};
                if (validate_normal_move(normal_move).has_value()) {
                    moves.push_back(normal_move);
                }
                
                // Check promotion possibility
                if (PieceMovement::can_promote(piece, from, to)) {
                    Move promotion_move{from, to, true};
                    if (validate_normal_move(promotion_move).has_value()) {
                        moves.push_back(promotion_move);
                    }
                }
            }
        }
    }
    
    // Generate drop moves
    const auto& hand = m_board_state.get_hand(player);
    const std::array<PieceType, 7> piece_types = {
        PieceType::Rook, PieceType::Bishop, PieceType::Gold,
        PieceType::Silver, PieceType::Knight, PieceType::Lance, PieceType::Pawn
    };
    
    for (size_t i = 0; i < piece_types.size(); ++i) {
        if (hand[i] > 0) {
            for (int row = 0; row < BOARD_ROWS; ++row) {
                for (int col = 0; col < BOARD_COLS; ++col) {
                    Position to{static_cast<int8_t>(row), static_cast<int8_t>(col)};
                    Move drop_move{to, piece_types[i]};
                    if (validate_drop_move(drop_move).has_value()) {
                        moves.push_back(drop_move);
                    }
                }
            }
        }
    }
    
    return moves;
}

BoardState GameLogic::simulate_move(const Move& move) const {
    BoardState test_state = m_board_state;
    
    if (move.is_drop) {
        Piece piece{move.drop_piece, test_state.current_player()};
        test_state.remove_from_hand(test_state.current_player(), move.drop_piece);
        test_state.set_piece(move.to, piece);
    } else {
        Piece piece = test_state.get_piece(move.from);
        const Piece& target = test_state.get_piece(move.to);
        
        if (!target.is_empty()) {
            test_state.add_to_hand(piece.owner, target.type);
        }
        
        if (move.is_promotion && piece.can_promote()) {
            piece.type = piece.promoted_type();
        }
        
        test_state.clear_piece(move.from);
        test_state.set_piece(move.to, piece);
    }
    
    return test_state;
}