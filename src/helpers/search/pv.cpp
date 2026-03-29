#include "pv.hpp"


void initPV(EngineSearchStuff& ess) {
    for (int a = 0; a < MAX_PLY; ++a) {
        ess.pvLength[a] = 0;
        for (int b = 0; b < MAX_PLY; ++b) {
            ess.pvTable[a][b] = chess::Move::NULL_MOVE;
        }
    }
}

void AddPV(chess::Move& new_move, EngineSearchStuff& ess, int ply) {
    ess.pvTable[ply][ply] = new_move;
    for (int pvi = ply + 1; pvi < ess.pvLength[ply + 1]; ++pvi) ess.pvTable[ply][pvi] = ess.pvTable[ply + 1][pvi];
    ess.pvLength[ply] = ess.pvLength[ply + 1];
}

std::string getPV(EngineSearchStuff& ess) {
    std::string pv = "";

    for (int i = 0; i < ess.pvLength[0]; ++i) {
        pv += " " + chess::uci::moveToUci(ess.pvTable[0][i]);
    }

    return pv;
}