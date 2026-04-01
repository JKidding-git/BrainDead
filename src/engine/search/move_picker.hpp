#pragma once

#include "../../include/chess.hpp"
#include <algorithm>


static constexpr int mvv_scores[6][6] = {
    {105, 104, 103, 102, 101, 0},
    {205, 204, 203, 202, 201, 0},
    {305, 304, 303, 302, 301, 0},
    {405, 404, 403, 402, 401, 0},
    {505, 504, 503, 502, 501, 0},
    {605, 604, 603, 602, 601, 0},
};

constexpr int mvv_lva(const chess::PieceType& attacker, const chess::PieceType& victim) {
    return mvv_scores[static_cast<int>(victim)][static_cast<int>(attacker)];
}

template <bool inQs>
inline void ScoreMoves(const chess::Board& board, chess::Movelist& moves) {
    for (int move_index = 0; move_index < moves.size(); move_index++) {
        if constexpr (inQs) {
            chess::Move move = moves[move_index];
            chess::PieceType attacker = board.at(move.from()).type();
            chess::PieceType victim = board.at(move.to()).type();

            moves[move_index].setScore(mvv_lva(attacker, victim));
        } else {
            int32_t move_score = 0;

            chess::Move move = moves[move_index];
            if (board.isCapture(move)) {
                chess::PieceType attacker = board.at(move.from()).type();
                chess::PieceType victim = board.at(move.to()).type();

                move_score = mvv_lva(attacker, victim) + 1000000;
            }

            moves[move_index].setScore(move_score);
        }
    }
}

void PickMove(chess::Movelist& moves, int current_move_index) {
    int best_move_index = current_move_index;

    for (int move_index = current_move_index + 1; move_index < moves.size(); move_index++) {
        if (moves[move_index].score() > moves[best_move_index].score()) {
            best_move_index = move_index;
        }
    }

    if (best_move_index != current_move_index) {
        std::swap(moves[current_move_index], moves[best_move_index]);
    }
}