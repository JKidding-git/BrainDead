#include "unsafe_squares.hpp"

chess::Bitboard GetAllKingUnsafeSquares(const chess::Board& board, chess::Color color) {
    chess::Bitboard mask = 0;

    chess::Square king_square = board.kingSq(color);
    chess::Bitboard occ = board.occ();

    chess::Bitboard queen_attacks_on_king_sq = chess::attacks::queen(king_square, occ);
    chess::Bitboard knight_attacks_on_king_sq = chess::attacks::knight(king_square);

    chess::Bitboard sliders = board.pieces(chess::PieceType::QUEEN, ~color) | board.pieces(chess::PieceType::ROOK, ~color) |
                             board.pieces(chess::PieceType::BISHOP, ~color);
    
    chess::Bitboard knights = board.pieces(chess::PieceType::KNIGHT, ~color);

    while (sliders) {
        chess::Square piece_square = sliders.pop();
        chess::Bitboard attacks = chess::attacks::queen(piece_square, occ);

        mask |= attacks & queen_attacks_on_king_sq;
    }

    while (knights) {
        chess::Square piece_square = knights.pop();
        chess::Bitboard attacks = chess::attacks::knight(piece_square);

        mask |= attacks & knight_attacks_on_king_sq;
    }

    return mask;
}