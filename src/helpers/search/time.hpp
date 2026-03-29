#pragma once
#include <chrono>
#include "controller.hpp"




using clk = std::chrono::_V2::steady_clock::time_point;

bool checkTime(bool iter, EngineController& ec, EngineSearchStuff& ess, clk t0);