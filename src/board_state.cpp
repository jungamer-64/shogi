#include "board_state.h"
#include "piece.h"
#include <algorithm>

size_t BoardState::pos_to_index(Position pos) const {
    return pos.row * BOARD_COLS + pos.col;
}

bool BoardState::is_valid_index(size_t index) const {
    return index < BOARD_SIZE;
}

BoardState::BoardState() {
    setup_initial_position();
}

void BoardState::setup_initial_position() {
    // Clear board
    m_board.fill(Piece{});
    
    // Clear hands
    for (auto& hand : m_hands) {
        hand.fill(0);
    }
    
    // Set initial position
    // Second player (top) pieces
    m_board[pos_to_index({0, 0})] = Piece{PieceType::Lance, Player::Second};
    m_board[pos_to_index({0, 1})] = Piece{PieceType::Knight, Player::Second};
    m_board[pos_to_index({0, 2})] = Piece{PieceType::Silver, Player::Second};
    m_board[pos_to_index({0, 3})] = Piece{PieceType::Gold, Player::Second};
    m_board[pos_to_index({0, 4})] = Piece{PieceType::King, Player::Second};
    m_board[pos_to_index({0, 5})] = Piece{PieceType::Gold, Player::Second};
    m_board[pos_to_index({0, 6})] = Piece{PieceType::Silver, Player::Second};
    m_board[pos_to_index({0, 7})] = Piece{PieceType::Knight, Player::Second};
    m_board[pos_to_index({0, 8})] = Piece{PieceType::Lance, Player::Second};
    
    m_board[pos_to_index({1, 1})] = Piece{PieceType::Bishop, Player::Second};
    m_board[pos_to_index({1, 7})] = Piece{PieceType::Rook, Player::Second};
    
    // Second player pawns
    for (int col = 0; col < BOARD_COLS; ++col) {
        m_board[pos_to_index({2, static_cast<int8_t>(col)})] = Piece{PieceType::Pawn, Player::Second};
    }
    
    // First player (bottom) pieces
    m_board[pos_to_index({8, 0})] = Piece{PieceType::Lance, Player::First};
    m_board[pos_to_index({8, 1})] = Piece{PieceType::Knight, Player::First};
    m_board[pos_to_index({8, 2})] = Piece{PieceType::Silver, Player::First};
    m_board[pos_to_index({8, 3})] = Piece{PieceType::Gold, Player::First};
    m_board[pos_to_index({8, 4})] = Piece{PieceType::King, Player::First};
    m_board[pos_to_index({8, 5})] = Piece{PieceType::Gold, Player::First};
    m_board[pos_to_index({8, 6})] = Piece{PieceType::Silver, Player::First};
    m_board[pos_to_index({8, 7})] = Piece{PieceType::Knight, Player::First};
    m_board[pos_to_index({8, 8})] = Piece{PieceType::Lance, Player::First};
    
    m_board[pos_to_index({7, 1})] = Piece{PieceType::Rook, Player::First};
    m_board[pos_to_index({7, 7})] = Piece{PieceType::Bishop, Player::First};
    
    // First player pawns
    for (int col = 0; col < BOARD_COLS; ++col) {
        m_board[pos_to_index({6, static_cast<int8_t>(col)})] = Piece{PieceType::Pawn, Player::First};
    }
    
    m_current_player = Player::First;
}

const Piece& BoardState::get_piece(Position pos) const {
    static const Piece empty_piece{};
    if (!pos.is_valid()) return empty_piece;
    
    size_t index = pos_to_index(pos);
    if (!is_valid_index(index)) return empty_piece;
    
    return m_board[index];
}

void BoardState::set_piece(Position pos, const Piece& piece) {
    if (!pos.is_valid()) return;
    
    size_t index = pos_to_index(pos);
    if (!is_valid_index(index)) return;
    
    m_board[index] = piece;
}

void BoardState::clear_piece(Position pos) {
    set_piece(pos, Piece{});
}

const Hand& BoardState::get_hand(Player player) const {
    return m_hands[static_cast<size_t>(player)];
}

void BoardState::add_to_hand(Player player, PieceType piece) {
    // Convert promoted pieces to their base type when captured
    PieceType base_piece = piece;
    switch (piece) {
        case PieceType::PromotedRook: base_piece = PieceType::Rook; break;
        case PieceType::PromotedBishop: base_piece = PieceType::Bishop; break;
        case PieceType::PromotedSilver: base_piece = PieceType::Silver; break;
        case PieceType::PromotedKnight: base_piece = PieceType::Knight; break;
        case PieceType::PromotedLance: base_piece = PieceType::Lance; break;
        case PieceType::PromotedPawn: base_piece = PieceType::Pawn; break;
        default: break;
    }
    
    size_t hand_index = piece_to_hand_index(base_piece);
    if (hand_index < 7) {
        m_hands[static_cast<size_t>(player)][hand_index]++;
    }
}

bool BoardState::remove_from_hand(Player player, PieceType piece) {
    size_t hand_index = piece_to_hand_index(piece);
    if (hand_index >= 7) return false;
    
    auto& hand = m_hands[static_cast<size_t>(player)];
    if (hand[hand_index] > 0) {
        hand[hand_index]--;
        return true;
    }
    return false;
}

uint8_t BoardState::get_hand_count(Player player, PieceType piece) const {
    size_t hand_index = piece_to_hand_index(piece);
    if (hand_index >= 7) return 0;
    
    return m_hands[static_cast<size_t>(player)][hand_index];
}

Position BoardState::find_king(Player player) const {
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            Position pos{static_cast<int8_t>(row), static_cast<int8_t>(col)};
            const Piece& piece = get_piece(pos);
            if (piece.type == PieceType::King && piece.owner == player) {
                return pos;
            }
        }
    }
    return INVALID_POSITION;
}

bool BoardState::is_square_attacked(Position pos, Player by_player) const {
    // Check if any piece of by_player can attack the given position
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            Position from{static_cast<int8_t>(row), static_cast<int8_t>(col)};
            const Piece& piece = get_piece(from);
            
            if (piece.is_empty() || piece.owner != by_player) {
                continue;
            }
            
            if (PieceMovement::is_valid_move(piece, from, pos, m_board)) {
                return true;
            }
        }
    }
    return false;
}

bool BoardState::is_in_check(Player player) const {
    Position king_pos = find_king(player);
    if (king_pos == INVALID_POSITION) {
        return false; // No king found (shouldn't happen)
    }
    
    return is_square_attacked(king_pos, other_player(player));
}