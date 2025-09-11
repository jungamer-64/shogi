#include "board_panel.h"
#include "game_logic.h"
#include "piece.h"
#include "common.h"
#include <wx/dcbuffer.h>
#include <wx/stdpaths.h>
#include <algorithm>

wxBEGIN_EVENT_TABLE(BoardPanel, wxPanel)
    EVT_PAINT(BoardPanel::OnPaint)
    EVT_SIZE(BoardPanel::OnSize)
    EVT_LEFT_DOWN(BoardPanel::OnMouseDown)
    EVT_LEFT_UP(BoardPanel::OnMouseUp)
    EVT_MOTION(BoardPanel::OnMouseMove)
    EVT_LEAVE_WINDOW(BoardPanel::OnMouseLeave)
wxEND_EVENT_TABLE()

BoardPanel::BoardPanel(wxWindow* parent, GameLogic* game_logic)
    : wxPanel(parent, wxID_ANY), m_game_logic(game_logic)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    
    m_svg_renderer = std::make_unique<SVGRenderer>();
    
    // Load SVG assets
    wxString asset_path = wxStandardPaths::Get().GetResourcesDir() + "/assets/";
    if (!wxDirExists(asset_path)) {
        asset_path = "./assets/";
    }
    
    m_svg_renderer->load_board_svg((asset_path + "board.svg").ToStdString());
    m_svg_renderer->load_pieces_svg((asset_path + "piece.svg").ToStdString());
    
    CalculateLayout();
}

void BoardPanel::OnPaint(wxPaintEvent& WXUNUSED(event)) {
    wxAutoBufferedPaintDC dc(this);
    
    auto gc = wxGraphicsContext::Create(dc);
    if (!gc) return;
    
    // Clear background
    gc->SetBrush(wxBrush(GetBackgroundColour()));
    wxSize size = GetSize();
    gc->DrawRectangle(0, 0, size.x, size.y);
    
    DrawBoard(gc);
    DrawHighlights(gc);
    DrawPieces(gc);
    DrawHands(gc);
    DrawGameInfo(gc);
    
    delete gc;
}

void BoardPanel::OnSize(wxSizeEvent& event) {
    CalculateLayout();
    Refresh();
    event.Skip();
}

void BoardPanel::OnMouseDown(wxMouseEvent& event) {
    Position pos = GetSquareFromPoint(event.GetPosition());
    
    if (pos.is_valid()) {
        if (m_selected_square == pos) {
            // Deselect if clicking the same square
            m_selected_square = INVALID_POSITION;
            m_highlighted_squares.clear();
        } else {
            OnSquareClicked(pos);
            OnSquareDragStart(pos);
        }
    }
    
    m_last_mouse_pos = event.GetPosition();
    Refresh();
}

void BoardPanel::OnMouseUp(wxMouseEvent& event) {
    if (m_dragging) {
        Position pos = GetSquareFromPoint(event.GetPosition());
        OnSquareDragEnd(pos);
        m_dragging = false;
    }
    
    Refresh();
}

void BoardPanel::OnMouseMove(wxMouseEvent& event) {
    if (event.Dragging() && event.LeftIsDown()) {
        if (!m_dragging && m_drag_from.is_valid()) {
            m_dragging = true;
        }
    }
    
    m_last_mouse_pos = event.GetPosition();
    
    if (m_dragging) {
        Refresh(); // Update for drag visualization
    }
}

void BoardPanel::OnMouseLeave(wxMouseEvent& WXUNUSED(event)) {
    if (m_dragging) {
        m_dragging = false;
        Refresh();
    }
}

void BoardPanel::CalculateLayout() {
    wxSize panel_size = GetSize();
    
    // Calculate board size (keep aspect ratio)
    int board_size = std::min(panel_size.x * 2 / 3, panel_size.y - 100);
    
    m_board_rect = wxRect(
        (panel_size.x - board_size) / 4,
        (panel_size.y - board_size) / 2,
        board_size,
        board_size
    );
    
    m_square_size = wxSize(board_size / 9, board_size / 9);
}

Position BoardPanel::GetSquareFromPoint(const wxPoint& point) const {
    if (!m_board_rect.Contains(point)) {
        return INVALID_POSITION;
    }
    
    int col = (point.x - m_board_rect.x) / m_square_size.x;
    int row = (point.y - m_board_rect.y) / m_square_size.y;
    
    if (col >= 0 && col < BOARD_COLS && row >= 0 && row < BOARD_ROWS) {
        return Position(row, col);
    }
    
    return INVALID_POSITION;
}

wxPoint BoardPanel::GetSquareCenter(Position pos) const {
    if (!pos.is_valid()) return wxPoint(-1, -1);
    
    int x = m_board_rect.x + pos.col * m_square_size.x + m_square_size.x / 2;
    int y = m_board_rect.y + pos.row * m_square_size.y + m_square_size.y / 2;
    
    return wxPoint(x, y);
}

wxRect BoardPanel::GetSquareRect(Position pos) const {
    if (!pos.is_valid()) return wxRect();
    
    int x = m_board_rect.x + pos.col * m_square_size.x;
    int y = m_board_rect.y + pos.row * m_square_size.y;
    
    return wxRect(x, y, m_square_size.x, m_square_size.y);
}

void BoardPanel::DrawBoard(wxGraphicsContext* gc) {
    if (!gc || !m_svg_renderer) return;
    
    m_svg_renderer->render_board(gc, m_board_rect);
}

void BoardPanel::DrawPieces(wxGraphicsContext* gc) {
    if (!gc || !m_game_logic || !m_svg_renderer) return;
    
    const auto& board_state = m_game_logic->get_board_state();
    
    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            Position pos{static_cast<int8_t>(row), static_cast<int8_t>(col)};
            const Piece& piece = board_state.get_piece(pos);
            
            if (piece.is_empty()) continue;
            
            // Skip piece being dragged
            if (m_dragging && pos == m_drag_from) continue;
            
            wxRect square_rect = GetSquareRect(pos);
            wxPoint piece_pos = square_rect.GetTopLeft();
            wxSize piece_size = square_rect.GetSize();
            
            // Add some padding
            int padding = 4;
            piece_pos.x += padding;
            piece_pos.y += padding;
            piece_size.x -= 2 * padding;
            piece_size.y -= 2 * padding;
            
            m_svg_renderer->render_piece(gc, piece.type, piece.owner, piece_pos, piece_size);
        }
    }
    
    // Draw dragged piece
    if (m_dragging && m_drag_from.is_valid()) {
        const Piece& piece = board_state.get_piece(m_drag_from);
        if (!piece.is_empty()) {
            wxSize piece_size(m_square_size.x - 8, m_square_size.y - 8);
            wxPoint piece_pos(m_last_mouse_pos.x - piece_size.x / 2,
                             m_last_mouse_pos.y - piece_size.y / 2);
            
            m_svg_renderer->render_piece(gc, piece.type, piece.owner, piece_pos, piece_size);
        }
    }
}

void BoardPanel::DrawHighlights(wxGraphicsContext* gc) {
    if (!gc) return;
    
    // Draw selected square highlight
    if (m_selected_square.is_valid()) {
        wxRect rect = GetSquareRect(m_selected_square);
        gc->SetBrush(wxBrush(m_highlight_color));
        gc->SetPen(wxPen(m_highlight_color, 3));
        gc->DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
    
    // Draw legal move highlights
    for (const auto& pos : m_highlighted_squares) {
        wxRect rect = GetSquareRect(pos);
        gc->SetBrush(wxBrush(m_legal_move_color));
        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    }
}

void BoardPanel::DrawHands(wxGraphicsContext* gc) {
    if (!gc || !m_game_logic) return;
    
    const auto& board_state = m_game_logic->get_board_state();
    
    // Draw first player's hand (bottom right)
    const auto& first_hand = board_state.get_hand(Player::First);
    int hand_x = m_board_rect.GetRight() + 20;
    int hand_y = m_board_rect.GetBottom() - 200;
    
    wxFont hand_font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    gc->SetFont(hand_font, wxColour(0, 0, 0));
    gc->DrawText("First Player Hand:", hand_x, hand_y);
    
    hand_y += 25;
    const std::array<std::string, 7> piece_names = {"飛", "角", "金", "銀", "桂", "香", "歩"};
    for (size_t i = 0; i < first_hand.size(); ++i) {
        if (first_hand[i] > 0) {
            std::string text = std::format("{}: {}", piece_names[i], first_hand[i]);
            gc->DrawText(text, hand_x, hand_y);
            hand_y += 20;
        }
    }
    
    // Draw second player's hand (top right)
    const auto& second_hand = board_state.get_hand(Player::Second);
    hand_y = m_board_rect.GetTop();
    
    gc->DrawText("Second Player Hand:", hand_x, hand_y);
    hand_y += 25;
    
    for (size_t i = 0; i < second_hand.size(); ++i) {
        if (second_hand[i] > 0) {
            std::string text = std::format("{}: {}", piece_names[i], second_hand[i]);
            gc->DrawText(text, hand_x, hand_y);
            hand_y += 20;
        }
    }
}

void BoardPanel::DrawGameInfo(wxGraphicsContext* gc) {
    if (!gc || !m_game_logic) return;
    
    // Draw current player info
    wxFont font(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    gc->SetFont(font, wxColour(0, 0, 0));
    
    std::string current_player_text = std::format("Current Player: {}",
        m_game_logic->get_current_player() == Player::First ? "First" : "Second");
    
    gc->DrawText(current_player_text, 20, 20);
    
    // Draw game status
    if (m_game_logic->is_in_check(m_game_logic->get_current_player())) {
        gc->SetBrush(wxBrush(wxColour(255, 0, 0)));
        gc->DrawText("CHECK!", 20, 45);
    }
    
    GameResult result = m_game_logic->get_game_result();
    if (result != GameResult::None) {
        std::string result_text;
        switch (result) {
            case GameResult::FirstPlayerWin: result_text = "First Player Wins!"; break;
            case GameResult::SecondPlayerWin: result_text = "Second Player Wins!"; break;
            case GameResult::Draw: result_text = "Draw!"; break;
            default: break;
        }
        
        gc->SetBrush(wxBrush(wxColour(0, 128, 0)));
        gc->DrawText(result_text, 20, 70);
    }
}

void BoardPanel::OnSquareClicked(Position pos) {
    if (!m_game_logic || !pos.is_valid()) return;
    
    const auto& board_state = m_game_logic->get_board_state();
    const Piece& clicked_piece = board_state.get_piece(pos);
    
    if (m_selected_square.is_valid()) {
        // Try to make a move
        const Piece& selected_piece = board_state.get_piece(m_selected_square);
        
        if (!selected_piece.is_empty() && selected_piece.owner == board_state.current_player()) {
            ::Move move{m_selected_square, pos};
            
            // Check if promotion is possible
            if (PieceMovement::can_promote(selected_piece, m_selected_square, pos)) {
                // For now, always promote when possible (TODO: add promotion dialog)
                move.is_promotion = true;
            }
            
            auto result = m_game_logic->make_move(move);
            if (result) {
                m_selected_square = INVALID_POSITION;
                m_highlighted_squares.clear();
                
                // Update parent frame status
                if (auto* parent = dynamic_cast<wxFrame*>(GetParent()->GetParent())) {
                    std::string status = std::format("Turn: {}", 
                        m_game_logic->get_current_player() == Player::First ? "First Player" : "Second Player");
                    parent->SetStatusText(status, 1);
                }
                return;
            }
        }
    }
    
    // Select new piece
    if (!clicked_piece.is_empty() && clicked_piece.owner == board_state.current_player()) {
        m_selected_square = pos;
        UpdateHighlights();
    } else {
        m_selected_square = INVALID_POSITION;
        m_highlighted_squares.clear();
    }
}

void BoardPanel::OnSquareDragStart(Position pos) {
    if (!m_game_logic || !pos.is_valid()) return;
    
    const auto& board_state = m_game_logic->get_board_state();
    const Piece& piece = board_state.get_piece(pos);
    
    if (!piece.is_empty() && piece.owner == board_state.current_player()) {
        m_drag_from = pos;
        m_selected_square = pos;
        UpdateHighlights();
    }
}

void BoardPanel::OnSquareDragEnd(Position pos) {
    if (!m_game_logic || !m_drag_from.is_valid()) return;
    
    if (pos.is_valid() && pos != m_drag_from) {
        const auto& board_state = m_game_logic->get_board_state();
        const Piece& piece = board_state.get_piece(m_drag_from);
        
        ::Move move{m_drag_from, pos};
        
        // Check if promotion is possible
        if (PieceMovement::can_promote(piece, m_drag_from, pos)) {
            move.is_promotion = true; // Auto-promote for now
        }
        
        auto result = m_game_logic->make_move(move);
        if (result) {
            m_selected_square = INVALID_POSITION;
            m_highlighted_squares.clear();
            
            // Update parent frame status
            if (auto* parent = dynamic_cast<wxFrame*>(GetParent()->GetParent())) {
                std::string status = std::format("Turn: {}", 
                    m_game_logic->get_current_player() == Player::First ? "First Player" : "Second Player");
                parent->SetStatusText(status, 1);
            }
        }
    }
    
    m_drag_from = INVALID_POSITION;
}

void BoardPanel::UpdateHighlights() {
    m_highlighted_squares.clear();
    
    if (m_game_logic && m_selected_square.is_valid()) {
        m_highlighted_squares = m_game_logic->get_legal_destinations(m_selected_square);
    }
}