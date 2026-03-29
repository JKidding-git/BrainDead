#pragma once
#include "controller.hpp"

void initPV(EngineSearchStuff& ess);
void AddPV(chess::Move& new_move, EngineSearchStuff& ess, int ply);
std::string getPV(EngineSearchStuff& ess);
