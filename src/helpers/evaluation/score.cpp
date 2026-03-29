#include "score.hpp"


int getPhase(const chess::Board& board) {
    chess::Bitboard occ = board.occ();
    int phase = 0;

    while (occ) {
        const int idx = occ.pop();
        const chess::Piece piece = board.at(idx);

        switch (piece) {
            case chess::Piece(chess::Piece::WHITEKNIGHT):
            case chess::Piece(chess::Piece::WHITEBISHOP):
            case chess::Piece(chess::Piece::BLACKKNIGHT):
            case chess::Piece(chess::Piece::BLACKBISHOP):
                phase += 1;
                break;
            case chess::Piece(chess::Piece::WHITEROOK):
            case chess::Piece(chess::Piece::BLACKROOK):
                phase += 2;
                break;
            case chess::Piece(chess::Piece::WHITEQUEEN):
            case chess::Piece(chess::Piece::BLACKQUEEN):
                phase += 4;
                break;
        }
    }

    return phase;
}

int32_t Blend(const chess::Board& board, score& s) {
    int32_t mg = s.middlegame;
    int32_t eg = s.endgame;

    int phase = getPhase(board);
    if (phase > 24) phase = 24;
    int result = (mg * phase + eg * (24 - phase)) / 24;
    return result;
}