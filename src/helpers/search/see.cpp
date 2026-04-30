#include "see.hpp"

static constexpr int values[7] = {100, 325, 350, 500, 900, 0, 0};

chess::Bitboard GetLeastValuablePieceSQ(const chess::Board& board, chess::Color color, chess::Bitboard attacks_bits, chess::PieceType &piece) {
    for (chess::PieceType p : {chess::PieceType::PAWN, chess::PieceType::KNIGHT, chess::PieceType::BISHOP, chess::PieceType::ROOK, chess::PieceType::QUEEN, chess::PieceType::KING}) {
        chess::Bitboard subsets = attacks_bits & board.pieces(p, color);
        if (subsets) {
            piece = p;
            return chess::Bitboard::fromSquare(subsets.pop());
        }
    }

    return 0;
}

int SEE(const chess::Board& board, chess::Square from, chess::Square to, chess::PieceType target, chess::PieceType a_piece, chess::Color color) {
    int gain[32], depth = 0;

    const chess::Bitboard sliders = board.pieces(chess::PieceType::BISHOP) | board.pieces(chess::PieceType::ROOK) | board.pieces(chess::PieceType::QUEEN);

    chess::Bitboard may_xray = board.pieces(chess::PieceType::PAWN) | board.pieces(chess::PieceType::BISHOP) | board.pieces(chess::PieceType::ROOK) | board.pieces(chess::PieceType::QUEEN);
    chess::Bitboard from_bit = chess::Bitboard::fromSquare(from);
    chess::Bitboard occ = board.occ();
    chess::Bitboard attacks_bits = chess::attacks::attackers(board, chess::Color::WHITE, to, occ) | chess::attacks::attackers(board, chess::Color::BLACK, to, occ);

    gain[depth] = values[static_cast<int>(target)];

    do {
        depth++;
        color = ~color;
        gain[depth] = values[static_cast<int>(a_piece)] - gain[depth - 1];

        attacks_bits ^= from_bit;
        occ          ^= from_bit;

        if (from_bit & may_xray) {
            attacks_bits |= (
                (chess::attacks::attackers(board, chess::Color::WHITE, to, occ) |
                chess::attacks::attackers(board, chess::Color::BLACK, to, occ))
                & sliders
            );
        }

        from_bit = GetLeastValuablePieceSQ(board, color, attacks_bits, a_piece);

    } while (from_bit);
    
    while (--depth) gain[depth - 1] = -std::max(-gain[depth - 1], gain[depth]);

    return gain[0];
}