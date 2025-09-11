#pragma once

#include "core/board/board.h"
#include <QString>
#include <map>

namespace shogi::core {

/**
 * @brief Predefined piece handicap configurations
 */
enum class KomaochiType {
    None,
    Kyou,        // Lance handicap
    Kaku,        // Bishop handicap
    Hisha,       // Rook handicap
    HishaKyou,   // Rook and Lance
    Nimai,       // Rook and Bishop
    Yonmai,      // Four pieces
    Rokmai,      // Six pieces
    Hachmai,     // Eight pieces
    Jumai,       // Ten pieces
    Custom       // User-defined
};

/**
 * @brief Manages piece handicap (Komaochi) setups
 */
class KomaochiManager {
public:
    KomaochiManager() = default;
    
    // Apply handicap to board
    void applyHandicap(Board& board, KomaochiType type) const;
    void applyCustomHandicap(Board& board, const std::map<Position, bool>& removedPieces) const;
    
    // Get handicap information
    QString getHandicapName(KomaochiType type) const;
    QString getHandicapDescription(KomaochiType type) const;
    
    // Validate handicap
    bool isValidHandicap(const Board& board) const;

private:
    void removeKyou(Board& board) const;
    void removeKaku(Board& board) const;
    void removeHisha(Board& board) const;
    void removeHishaKyou(Board& board) const;
    void removeNimai(Board& board) const;
    void removeYonmai(Board& board) const;
    void removeRokmai(Board& board) const;
    void removeHachmai(Board& board) const;
    void removeJumai(Board& board) const;
};

} // namespace shogi::core