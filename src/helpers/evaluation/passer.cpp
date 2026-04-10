#include "passer.hpp"

chess::Bitboard GeneratePasserMask(const chess::Board& board, chess::Color color, chess::Bitboard pawns) {
    chess::Bitboard mask = 0;

    if (color == chess::Color::WHITE) {
        mask |= pawns << 8;

        // This will not use en passant squares.
        mask |= chess::attacks::pawnLeftAttacks<chess::Color::WHITE>(pawns);
        mask |= chess::attacks::pawnRightAttacks<chess::Color::WHITE>(pawns);
        mask = FrontFill(mask);
    } else {
        mask |= pawns >> 8;

        // This will not use en passant squares.
        mask |= chess::attacks::pawnLeftAttacks<chess::Color::BLACK>(pawns);
        mask |= chess::attacks::pawnRightAttacks<chess::Color::BLACK>(pawns);
        mask = BackFill(mask);
    }

    return mask;
}

chess::Bitboard GetAllPassers(const chess::Board& board, chess::Color color) {
    chess::Bitboard pawns = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard opponent_pawns = board.pieces(chess::PieceType::PAWN, ~color);
    chess::Bitboard mask = GeneratePasserMask(board, ~color, opponent_pawns);

    return pawns & ~mask;
}