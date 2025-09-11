#include "piece.h"

namespace shogi::core {

Piece::Piece(PieceType type, Player player) : type_(type), player_(player) {}

bool Piece::isPromoted() const {
    return type_ >= PieceType::PromotedPawn && type_ <= PieceType::PromotedRook;
}

bool Piece::canPromote() const {
    if (isEmpty() || isPromoted()) {
        return false;
    }
    
    // King and Gold cannot promote
    return type_ != PieceType::King && type_ != PieceType::Gold;
}

Piece Piece::promote() const {
    if (!canPromote()) {
        return *this;
    }
    
    PieceType promotedType;
    switch (type_) {
        case PieceType::Pawn:   promotedType = PieceType::PromotedPawn; break;
        case PieceType::Lance:  promotedType = PieceType::PromotedLance; break;
        case PieceType::Knight: promotedType = PieceType::PromotedKnight; break;
        case PieceType::Silver: promotedType = PieceType::PromotedSilver; break;
        case PieceType::Bishop: promotedType = PieceType::PromotedBishop; break;
        case PieceType::Rook:   promotedType = PieceType::PromotedRook; break;
        default: return *this;
    }
    
    return Piece(promotedType, player_);
}

Piece Piece::demote() const {
    if (!isPromoted()) {
        return *this;
    }
    
    PieceType demotedType;
    switch (type_) {
        case PieceType::PromotedPawn:   demotedType = PieceType::Pawn; break;
        case PieceType::PromotedLance:  demotedType = PieceType::Lance; break;
        case PieceType::PromotedKnight: demotedType = PieceType::Knight; break;
        case PieceType::PromotedSilver: demotedType = PieceType::Silver; break;
        case PieceType::PromotedBishop: demotedType = PieceType::Bishop; break;
        case PieceType::PromotedRook:   demotedType = PieceType::Rook; break;
        default: return *this;
    }
    
    return Piece(demotedType, player_);
}

std::string Piece::toString() const {
    if (isEmpty()) {
        return " ";
    }
    
    std::string result;
    switch (type_) {
        case PieceType::Pawn:           result = "P"; break;
        case PieceType::Lance:          result = "L"; break;
        case PieceType::Knight:         result = "N"; break;
        case PieceType::Silver:         result = "S"; break;
        case PieceType::Gold:           result = "G"; break;
        case PieceType::Bishop:         result = "B"; break;
        case PieceType::Rook:           result = "R"; break;
        case PieceType::King:           result = "K"; break;
        case PieceType::PromotedPawn:   result = "+P"; break;
        case PieceType::PromotedLance:  result = "+L"; break;
        case PieceType::PromotedKnight: result = "+N"; break;
        case PieceType::PromotedSilver: result = "+S"; break;
        case PieceType::PromotedBishop: result = "+B"; break;
        case PieceType::PromotedRook:   result = "+R"; break;
        default: result = "?"; break;
    }
    
    // Lowercase for Gote (white)
    if (player_ == Player::Gote) {
        for (char& c : result) {
            if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
        }
    }
    
    return result;
}

bool Piece::operator==(const Piece& other) const {
    return type_ == other.type_ && player_ == other.player_;
}

bool Piece::operator!=(const Piece& other) const {
    return !(*this == other);
}

} // namespace shogi::core