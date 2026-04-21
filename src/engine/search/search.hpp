#pragma once

#include "../../helpers/search/time.hpp"
#include "../../helpers/search/pv.hpp"
#include "../evaluation/evaluate.hpp"

int qSearch(chess::Board& board, int alpha, int beta, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0, Trace& trace);
int alphaBeta(chess::Board& board, int alpha, int beta, int depth, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0, Trace& trace);
void IterativeDeepening(chess::Board& board, EngineSearchStuff& ess, EngineController& ec);