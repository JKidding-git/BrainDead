#include "isolated.hpp"

chess::Bitboard GetAllIsolatedPawns(const chess::Board& board, chess::Color color) {
    chess::Bitboard pawns = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard mask = 0;

    if (color == chess::Color::WHITE) {
        mask |= chess::attacks::pawnLeftAttacks<chess::Color::WHITE>(pawns);
        mask |= chess::attacks::pawnRightAttacks<chess::Color::WHITE>(pawns);

        mask |= FrontFill(mask);
        mask |= BackFill(mask);
    } else {
        mask |= chess::attacks::pawnLeftAttacks<chess::Color::BLACK>(pawns);
        mask |= chess::attacks::pawnRightAttacks<chess::Color::BLACK>(pawns);

        mask |= FrontFill(mask);
        mask |= BackFill(mask);
    }

    return pawns & ~mask;
}