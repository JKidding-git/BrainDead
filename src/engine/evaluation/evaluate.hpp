#pragma once
#include "../../helpers/evaluation/score.hpp"
#include "../../include/chess.hpp"


struct Trace {
    // the last [2] is just the color index.
    int32_t piece_values[6][2];
    int32_t psqt[6][64][2];
    int32_t passer_bonuses[8][2];
    int32_t isolated_pawn_penalty[9][2];
    int32_t backward_pawn_penalty[2];
    int32_t king_pawn_shield_bonus[2];
    int32_t unsafe_square_penalty[2];
    int32_t bishop_pair_bonus[2];
};

int evaluate(const chess::Board& board, Trace& trace);