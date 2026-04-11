#include "king_shield.hpp"

chess::Bitboard GenerateKingShieldMask(const chess::Bitboard& king_sq_bit, chess::Color color) {
    chess::Bitboard mask = 0;

    if (color == chess::Color::WHITE) {
        mask |= chess::attacks::pawnLeftAttacks<chess::Color::WHITE>(king_sq_bit);
        mask |= chess::attacks::pawnRightAttacks<chess::Color::WHITE>(king_sq_bit);
        mask |= king_sq_bit << 8;
        mask |= mask << 8;
    } else {
        mask |= chess::attacks::pawnLeftAttacks<chess::Color::BLACK>(king_sq_bit);
        mask |= chess::attacks::pawnRightAttacks<chess::Color::BLACK>(king_sq_bit);
        mask |= king_sq_bit >> 8;
        mask |= mask >> 8;
    }
    
    return mask;
}

chess::Bitboard GetCastlingSquares(chess::Color color) {
    if (color == chess::Color::WHITE) {
        return chess::Bitboard::fromSquare(chess::Square::SQ_G1) | chess::Bitboard::fromSquare(chess::Square::SQ_C1);
    }

    return chess::Bitboard::fromSquare(chess::Square::SQ_G8) | chess::Bitboard::fromSquare(chess::Square::SQ_C8);
}