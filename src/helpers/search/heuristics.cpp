#include "heuristics.hpp"

void AddKiller(chess::Move move, EngineSearchStuff& ess, int ply) {
    if (move == chess::Move::NULL_MOVE) return;

    if (ess.killerMoves[0][ply] != move) {
        ess.killerMoves[1][ply] = ess.killerMoves[0][ply];
        ess.killerMoves[0][ply] = move;
    }
}