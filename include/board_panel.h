#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
#include <memory>
#include "common.h"
#include "svg_renderer.h"

class GameLogic;

class BoardPanel : public wxPanel {
public:
    BoardPanel(wxWindow* parent, GameLogic* game_logic);
    
    void SetGameLogic(GameLogic* game_logic) { m_game_logic = game_logic; }
    
protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    
private:
    GameLogic* m_game_logic{nullptr};
    std::unique_ptr<SVGRenderer> m_svg_renderer;
    
    // Interaction state
    bool m_dragging{false};
    Position m_selected_square{INVALID_POSITION};
    Position m_drag_from{INVALID_POSITION};
    wxPoint m_last_mouse_pos;
    std::vector<Position> m_highlighted_squares;
    
    // Layout
    wxRect m_board_rect;
    wxSize m_square_size;
    
    // Colors and styling
    wxColour m_highlight_color{255, 255, 0, 128}; // Semi-transparent yellow
    wxColour m_legal_move_color{0, 255, 0, 64};   // Semi-transparent green
    wxColour m_last_move_color{255, 0, 0, 64};    // Semi-transparent red
    
    // Helper functions
    void CalculateLayout();
    Position GetSquareFromPoint(const wxPoint& point) const;
    wxPoint GetSquareCenter(Position pos) const;
    wxRect GetSquareRect(Position pos) const;
    
    void DrawBoard(wxGraphicsContext* gc);
    void DrawPieces(wxGraphicsContext* gc);
    void DrawHighlights(wxGraphicsContext* gc);
    void DrawHands(wxGraphicsContext* gc);
    void DrawGameInfo(wxGraphicsContext* gc);
    
    void OnSquareClicked(Position pos);
    void OnSquareDragStart(Position pos);
    void OnSquareDragEnd(Position pos);
    
    void UpdateHighlights();
    
    wxDECLARE_EVENT_TABLE();
};