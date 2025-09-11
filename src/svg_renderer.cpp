#include "svg_renderer.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>

SVGRenderer::SVGRenderer() {
}

SVGRenderer::~SVGRenderer() {
}

bool SVGRenderer::load_board_svg(const std::string& filepath) {
    m_board_loaded = load_svg_to_bitmap(filepath, m_board_bitmap);
    return m_board_loaded;
}

bool SVGRenderer::load_pieces_svg(const std::string& filepath) {
    m_pieces_loaded = load_svg_to_bitmap(filepath, m_pieces_bitmap);
    return m_pieces_loaded;
}

bool SVGRenderer::load_svg_to_bitmap(const std::string& filepath, wxBitmap& bitmap) {
    // For now, load as a regular image since SVG support varies
    // In a full implementation, you'd use librsvg or similar
    wxImage image;
    if (image.LoadFile(filepath, wxBITMAP_TYPE_ANY)) {
        bitmap = wxBitmap(image);
        return true;
    }
    
    // Fallback: create a simple colored bitmap
    bitmap = wxBitmap(540, 600);
    wxMemoryDC dc(bitmap);
    dc.SetBackground(wxBrush(wxColour(245, 222, 179))); // Wheat color
    dc.Clear();
    
    // Draw simple grid
    dc.SetPen(wxPen(wxColour(139, 69, 19), 1)); // Brown color
    for (int i = 0; i <= 9; ++i) {
        int x = i * 60;
        dc.DrawLine(x, 0, x, 540);
    }
    for (int i = 0; i <= 9; ++i) {
        int y = i * 60;
        dc.DrawLine(0, y, 540, y);
    }
    
    return true;
}

void SVGRenderer::render_board(wxGraphicsContext* gc, const wxRect& rect) {
    if (!m_board_loaded || !gc) {
        // Fallback rendering
        gc->SetBrush(wxBrush(wxColour(245, 222, 179)));
        gc->DrawRectangle(rect.x, rect.y, rect.width, rect.height);
        
        // Draw grid
        gc->SetPen(wxPen(wxColour(139, 69, 19), 1));
        double cell_width = rect.width / 9.0;
        double cell_height = rect.height / 9.0;
        
        for (int i = 0; i <= 9; ++i) {
            double x = rect.x + i * cell_width;
            gc->StrokeLine(x, rect.y, x, rect.y + rect.height);
        }
        for (int i = 0; i <= 9; ++i) {
            double y = rect.y + i * cell_height;
            gc->StrokeLine(rect.x, y, rect.x + rect.width, y);
        }
        return;
    }
    
    // Scale and draw the board bitmap
    gc->DrawBitmap(m_board_bitmap, rect.x, rect.y, rect.width, rect.height);
}

void SVGRenderer::render_piece(wxGraphicsContext* gc, PieceType type, Player player, 
                              const wxPoint& position, const wxSize& size) {
    if (!gc || type == PieceType::None) {
        return;
    }
    
    if (m_pieces_loaded) {
        wxRect sprite_rect = get_piece_sprite_rect(type, player);
        
        // Create a sub-bitmap from the sprite sheet
        wxBitmap sub_bitmap = m_pieces_bitmap.GetSubBitmap(sprite_rect);
        gc->DrawBitmap(sub_bitmap, position.x, position.y, size.x, size.y);
    } else {
        // Fallback: draw simple text representation
        gc->SetBrush(wxBrush(player == Player::First ? wxColour(255, 250, 205) : wxColour(240, 230, 140)));
        gc->SetPen(wxPen(wxColour(139, 69, 19), 2));
        gc->DrawRectangle(position.x, position.y, size.x, size.y);
        
        // Draw piece character
        wxFont font(size.y / 3, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        gc->SetFont(font, wxColour(139, 69, 19));
        
        std::string piece_char;
        switch (type) {
            case PieceType::King: piece_char = (player == Player::First) ? "玉" : "王"; break;
            case PieceType::Rook: piece_char = "飛"; break;
            case PieceType::Bishop: piece_char = "角"; break;
            case PieceType::Gold: piece_char = "金"; break;
            case PieceType::Silver: piece_char = "銀"; break;
            case PieceType::Knight: piece_char = "桂"; break;
            case PieceType::Lance: piece_char = "香"; break;
            case PieceType::Pawn: piece_char = "歩"; break;
            case PieceType::PromotedRook: piece_char = "竜"; break;
            case PieceType::PromotedBishop: piece_char = "馬"; break;
            case PieceType::PromotedSilver: piece_char = "成銀"; break;
            case PieceType::PromotedKnight: piece_char = "成桂"; break;
            case PieceType::PromotedLance: piece_char = "成香"; break;
            case PieceType::PromotedPawn: piece_char = "と"; break;
            default: piece_char = "?"; break;
        }
        
        wxDouble text_width, text_height;
        gc->GetTextExtent(piece_char, &text_width, &text_height);
        
        double text_x = position.x + (size.x - text_width) / 2;
        double text_y = position.y + (size.y - text_height) / 2;
        
        if (player == Player::Second) {
            // Rotate text for second player
            gc->PushState();
            gc->Translate(position.x + size.x/2, position.y + size.y/2);
            gc->Rotate(M_PI);
            gc->DrawText(piece_char, -text_width/2, -text_height/2);
            gc->PopState();
        } else {
            gc->DrawText(piece_char, text_x, text_y);
        }
    }
}

wxRect SVGRenderer::get_piece_sprite_rect(PieceType type, Player player) const {
    int sprite_index = get_piece_sprite_index(type, player);
    
    int row = sprite_index / SPRITES_PER_ROW;
    int col = sprite_index % SPRITES_PER_ROW;
    
    return wxRect(col * SPRITE_WIDTH, row * SPRITE_HEIGHT, SPRITE_WIDTH, SPRITE_HEIGHT);
}

int SVGRenderer::get_piece_sprite_index(PieceType type, Player player) const {
    // Map piece types to sprite indices
    int base_index = 0;
    
    switch (type) {
        case PieceType::King: base_index = 0; break;
        case PieceType::Rook: base_index = 1; break;
        case PieceType::Bishop: base_index = 2; break;
        case PieceType::Gold: base_index = 3; break;
        case PieceType::Silver: base_index = 4; break;
        case PieceType::Knight: base_index = 5; break;
        case PieceType::Lance: base_index = 6; break;
        case PieceType::Pawn: base_index = 7; break;
        case PieceType::PromotedRook: base_index = 8; break;
        case PieceType::PromotedBishop: base_index = 9; break;
        case PieceType::PromotedSilver: base_index = 10; break;
        case PieceType::PromotedKnight: base_index = 11; break;
        case PieceType::PromotedLance: base_index = 12; break;
        case PieceType::PromotedPawn: base_index = 13; break;
        default: base_index = 0; break;
    }
    
    // Offset for second player sprites
    if (player == Player::Second) {
        base_index += 14; // Second row starts at index 14
    }
    
    return base_index;
}