#include "score.hpp"

static constexpr uint8_t KNIGHTS_WEIGHT = 1;
static constexpr uint8_t BISHOPS_WEIGHT = 1;
static constexpr uint8_t ROOKS_WEIGHT   = 2;
static constexpr uint8_t QUEENS_WEIGHT  = 4;



int getPhase(const chess::Board& board) {
    int phase = 0;

    chess::Bitboard knights = board.pieces(chess::PieceType::KNIGHT);
    chess::Bitboard bishops = board.pieces(chess::PieceType::BISHOP);
    chess::Bitboard rooks   = board.pieces(chess::PieceType::ROOK);
    chess::Bitboard queens  = board.pieces(chess::PieceType::QUEEN);

    phase += KNIGHTS_WEIGHT * knights.count();
    phase += BISHOPS_WEIGHT * bishops.count();
    phase += ROOKS_WEIGHT   * rooks.count();
    phase += QUEENS_WEIGHT  * queens.count();

    return phase;
}

int32_t Blend(const chess::Board& board, score& value) {
    int32_t mg = value.middlegame;
    int32_t eg = value.endgame;

    int phase = getPhase(board);
    if (phase > 24) phase = 24;

    int result = (mg * phase + eg * (24 - phase)) / 24;
    return result;
}