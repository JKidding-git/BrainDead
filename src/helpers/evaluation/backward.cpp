#include "backward.hpp"

chess::Bitboard GetAllBackwardPawns(const chess::Board& board, chess::Color color) {
    chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
    chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

    if (color == chess::Color::WHITE) {
        chess::Bitboard white_attack = 0;
        chess::Bitboard black_attack = 0;

        white_attack |= chess::attacks::pawnLeftAttacks<chess::Color::WHITE>(white_pawns);
        white_attack |= chess::attacks::pawnRightAttacks<chess::Color::WHITE>(white_pawns);

        black_attack |= chess::attacks::pawnLeftAttacks<chess::Color::BLACK>(black_pawns);
        black_attack |= chess::attacks::pawnRightAttacks<chess::Color::BLACK>(black_pawns);

        chess::Bitboard dominated_stop_square = ~FrontFill(white_attack) & black_attack;
        chess::Bitboard white_backward_area = BackFill(dominated_stop_square);

        return white_backward_area & white_pawns;
    }

    chess::Bitboard white_attack = 0;
    chess::Bitboard black_attack = 0;

    white_attack |= chess::attacks::pawnLeftAttacks<chess::Color::WHITE>(white_pawns);
    white_attack |= chess::attacks::pawnRightAttacks<chess::Color::WHITE>(white_pawns);

    black_attack |= chess::attacks::pawnLeftAttacks<chess::Color::BLACK>(black_pawns);
    black_attack |= chess::attacks::pawnRightAttacks<chess::Color::BLACK>(black_pawns);

    chess::Bitboard dominated_stop_square = ~BackFill(black_attack) & white_attack;
    chess::Bitboard black_backward_area = FrontFill(dominated_stop_square);

    return black_backward_area & black_pawns;
}