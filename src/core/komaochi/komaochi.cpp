#include "komaochi.h"

namespace shogi::core {

void KomaochiManager::applyHandicap(Board& board, KomaochiType type) const {
    // Start with standard position
    board.setupInitialPosition();
    
    switch (type) {
        case KomaochiType::None:
            break;
        case KomaochiType::Kyou:
            removeKyou(board);
            break;
        case KomaochiType::Kaku:
            removeKaku(board);
            break;
        case KomaochiType::Hisha:
            removeHisha(board);
            break;
        case KomaochiType::HishaKyou:
            removeHishaKyou(board);
            break;
        case KomaochiType::Nimai:
            removeNimai(board);
            break;
        case KomaochiType::Yonmai:
            removeYonmai(board);
            break;
        case KomaochiType::Rokmai:
            removeRokmai(board);
            break;
        case KomaochiType::Hachmai:
            removeHachmai(board);
            break;
        case KomaochiType::Jumai:
            removeJumai(board);
            break;
        case KomaochiType::Custom:
            // Custom handicaps are applied separately
            break;
    }
}

void KomaochiManager::applyCustomHandicap(Board& board, const std::map<Position, bool>& removedPieces) const {
    board.setupInitialPosition();
    
    for (const auto& [pos, removed] : removedPieces) {
        if (removed) {
            board.setPiece(pos, Piece());
        }
    }
}

QString KomaochiManager::getHandicapName(KomaochiType type) const {
    switch (type) {
        case KomaochiType::None: return "平手";
        case KomaochiType::Kyou: return "香落ち";
        case KomaochiType::Kaku: return "角落ち";
        case KomaochiType::Hisha: return "飛車落ち";
        case KomaochiType::HishaKyou: return "飛香落ち";
        case KomaochiType::Nimai: return "二枚落ち";
        case KomaochiType::Yonmai: return "四枚落ち";
        case KomaochiType::Rokmai: return "六枚落ち";
        case KomaochiType::Hachmai: return "八枚落ち";
        case KomaochiType::Jumai: return "十枚落ち";
        case KomaochiType::Custom: return "カスタム";
        default: return "不明";
    }
}

QString KomaochiManager::getHandicapDescription(KomaochiType type) const {
    switch (type) {
        case KomaochiType::None: return "通常の対局";
        case KomaochiType::Kyou: return "上手の左香を除く";
        case KomaochiType::Kaku: return "上手の角を除く";
        case KomaochiType::Hisha: return "上手の飛車を除く";
        case KomaochiType::HishaKyou: return "上手の飛車と左香を除く";
        case KomaochiType::Nimai: return "上手の飛車と角を除く";
        case KomaochiType::Yonmai: return "上手の飛車、角、左香、右香を除く";
        case KomaochiType::Rokmai: return "上手の飛車、角、両香、両桂を除く";
        case KomaochiType::Hachmai: return "上手の飛車、角、両香、両桂、両銀を除く";
        case KomaochiType::Jumai: return "上手の飛車、角、両香、両桂、両銀、両金を除く";
        case KomaochiType::Custom: return "ユーザー定義の駒落ち";
        default: return "不明な駒落ち";
    }
}

bool KomaochiManager::isValidHandicap(const Board& board) const {
    // Basic validation - check if both kings are present
    bool senteKingFound = false;
    bool goteKingFound = false;
    
    for (int rank = 1; rank <= 9; ++rank) {
        for (int file = 1; file <= 9; ++file) {
            Piece piece = board.getPiece(Position(file, rank));
            if (piece.getType() == PieceType::King) {
                if (piece.getPlayer() == Player::Sente) {
                    senteKingFound = true;
                } else if (piece.getPlayer() == Player::Gote) {
                    goteKingFound = true;
                }
            }
        }
    }
    
    return senteKingFound && goteKingFound;
}

void KomaochiManager::removeKyou(Board& board) const {
    // Remove left lance (1,1)
    board.setPiece(Position(1, 1), Piece());
}

void KomaochiManager::removeKaku(Board& board) const {
    // Remove bishop (8,2)
    board.setPiece(Position(8, 2), Piece());
}

void KomaochiManager::removeHisha(Board& board) const {
    // Remove rook (2,2)
    board.setPiece(Position(2, 2), Piece());
}

void KomaochiManager::removeHishaKyou(Board& board) const {
    removeHisha(board);
    removeKyou(board);
}

void KomaochiManager::removeNimai(Board& board) const {
    removeHisha(board);
    removeKaku(board);
}

void KomaochiManager::removeYonmai(Board& board) const {
    removeNimai(board);
    // Remove both lances
    board.setPiece(Position(1, 1), Piece());
    board.setPiece(Position(9, 1), Piece());
}

void KomaochiManager::removeRokmai(Board& board) const {
    removeYonmai(board);
    // Remove both knights
    board.setPiece(Position(2, 1), Piece());
    board.setPiece(Position(8, 1), Piece());
}

void KomaochiManager::removeHachmai(Board& board) const {
    removeRokmai(board);
    // Remove both silvers
    board.setPiece(Position(3, 1), Piece());
    board.setPiece(Position(7, 1), Piece());
}

void KomaochiManager::removeJumai(Board& board) const {
    removeHachmai(board);
    // Remove both golds
    board.setPiece(Position(4, 1), Piece());
    board.setPiece(Position(6, 1), Piece());
}

} // namespace shogi::core