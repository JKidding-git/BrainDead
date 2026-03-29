#pragma once

#include "../../helpers/search/time.hpp"
#include "../../helpers/search/pv.hpp"

int qSearch(chess::Board& board, int alpha, int beta, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0);
int alphaBeta(chess::Board& board, int alpha, int beta, int depth, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0);
void IterativeDeepening(chess::Board& board, EngineSearchStuff& ess, EngineController& ec);