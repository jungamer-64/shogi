#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
#include <string>
#include <memory>
#include "common.h"

class SVGRenderer {
public:
    SVGRenderer();
    ~SVGRenderer();
    
    // Load SVG files
    bool load_board_svg(const std::string& filepath);
    bool load_pieces_svg(const std::string& filepath);
    
    // Render board
    void render_board(wxGraphicsContext* gc, const wxRect& rect);
    
    // Render pieces
    void render_piece(wxGraphicsContext* gc, PieceType type, Player player, 
                     const wxPoint& position, const wxSize& size);
    
    // Get piece sprite coordinates
    wxRect get_piece_sprite_rect(PieceType type, Player player) const;
    
private:
    // SVG data or bitmap cache
    wxBitmap m_board_bitmap;
    wxBitmap m_pieces_bitmap;
    
    bool m_board_loaded{false};
    bool m_pieces_loaded{false};
    
    // Piece sprite layout constants
    static constexpr int SPRITE_WIDTH = 60;
    static constexpr int SPRITE_HEIGHT = 60;
    static constexpr int SPRITES_PER_ROW = 13;
    
    // Helper functions
    bool load_svg_to_bitmap(const std::string& filepath, wxBitmap& bitmap);
    int get_piece_sprite_index(PieceType type, Player player) const;
};