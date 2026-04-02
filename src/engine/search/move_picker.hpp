#pragma once

#include "../../include/chess.hpp"
#include <algorithm>
#include "../../helpers/search/controller.hpp"



// bad capture = anything less than 6000.
static constexpr int mvv_scores[6][6] = {
    {6002, 20225, 20250, 20400, 20800, 26900},
    {4775, 6004, 20025, 20175, 20575, 26675},
    {4750, 4975, 6006, 20150, 20550, 26650},
    {4600, 4825, 4850, 6008, 20400, 26500},
    {4200, 4425, 4450, 4600, 6010, 26100},
    {3100, 3325, 3350, 3500, 3900, 26000},
};

static constexpr int CAPTURE_BONUS = 32'000;

constexpr int mvv_lva(const chess::PieceType& attacker, const chess::PieceType& victim) {
    return mvv_scores[static_cast<int>(attacker)][static_cast<int>(victim)];
}

template <bool inQs>
inline void ScoreMoves(const chess::Board& board, chess::Movelist& moves, EngineSearchStuff& ess, int ply) {
    for (int move_index = 0; move_index < moves.size(); move_index++) {
        if constexpr (inQs) {
            chess::Move& move = moves[move_index];
            chess::PieceType attacker = board.at(move.from()).type();
            chess::PieceType victim = board.at(move.to()).type();

            move.setScore(mvv_lva(attacker, victim));
        } else {
            

            chess::Move& move = moves[move_index];
            if (board.isCapture(move)) {
                chess::PieceType attacker = board.at(move.from()).type();
                chess::PieceType victim = board.at(move.to()).type();

                move.setScore(CAPTURE_BONUS + mvv_lva(attacker, victim));
                continue;
            } else {

                // Killer moves
                if (move == ess.killerMoves[0][ply]) {
                    move.setScore(CAPTURE_BONUS + 5000);
                    continue;
                } else if (move == ess.killerMoves[1][ply]) {
                    move.setScore(CAPTURE_BONUS + 4999);
                    continue;
                }
            }
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