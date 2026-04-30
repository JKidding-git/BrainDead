#pragma once

#include "../../include/chess.hpp"
#include <algorithm>
#include "../../helpers/search/controller.hpp"
#include "tt.hpp"
#include "../../helpers/search/see.hpp"



// bad capture = anything less than 6000.
static constexpr int mvv_scores[6][6] = {
    {6002, 20225, 20250, 20400, 20800, 26900},
    {4775, 6004, 20025, 20175, 20575, 26675},
    {4750, 4975, 6006, 20150, 20550, 26650},
    {4600, 4825, 4850, 6008, 20400, 26500},
    {4200, 4425, 4450, 4600, 6010, 26100},
    {3100, 3325, 3350, 3500, 3900, 26000},
};

static constexpr int TT_BONUS = 1'000'000;
static constexpr int CAPTURE_BONUS = 100'000;
static constexpr int BAD_CAPTURE_BONUS = -100'000;
static constexpr int KILLER_BONUS_1 = 90'000;
static constexpr int KILLER_BONUS_2 = 89'000;

constexpr int mvv_lva(const chess::PieceType& attacker, const chess::PieceType& victim) {
    return mvv_scores[static_cast<int>(attacker)][static_cast<int>(victim)];
}

// TODO: change this to score individual moves instead of the whole move list at once.
template <bool inQs>
inline void ScoreMoves(const chess::Board& board, chess::Movelist& moves, EngineSearchStuff& ess, int ply, chess::Move ttMove) {
    for (int move_index = 0; move_index < moves.size(); move_index++) {
        if constexpr (inQs) {
            chess::Move& move = moves[move_index];
            chess::PieceType attacker = board.at(move.from()).type();
            chess::PieceType victim = board.at(move.to()).type();

            int see_score = SEE(board, move.from(), move.to(), victim, attacker, board.sideToMove());
            if (see_score < 0) continue;

            move.setScore(mvv_lva(attacker, victim));
        } else {
            

            chess::Move& move = moves[move_index];

            if (move == ttMove && ttMove != chess::Move::NULL_MOVE) {
                move.setScore(TT_BONUS);
                continue;
            }


            if (board.isCapture(move)) {
                chess::PieceType attacker = board.at(move.from()).type();
                chess::PieceType victim = board.at(move.to()).type();

                int see_score = SEE(board, move.from(), move.to(), victim, attacker, board.sideToMove());

                if (see_score < 0) {
                    move.setScore(BAD_CAPTURE_BONUS + see_score);
                    continue;
                }

                move.setScore(CAPTURE_BONUS + mvv_lva(attacker, victim));
                continue;
            } else {

                // Killer moves
                if (move == ess.killerMoves[0][ply]) {
                    move.setScore(KILLER_BONUS_1);
                    continue;
                } else if (move == ess.killerMoves[1][ply]) {
                    move.setScore(KILLER_BONUS_2);
                    continue;
                }
            }

            bool side = board.sideToMove() == chess::Color::WHITE;
            int from = move.from().index();
            int to = move.to().index();
            move.setScore(ess.history[side][from][to]);
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