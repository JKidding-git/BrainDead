#pragma once

#include <atomic>
#include <cstdint>
#include <thread>
#include "../../include/chess.hpp"
#include "values.hpp"

inline std::thread search;

#define Load(type) (type.load(std::memory_order_relaxed))
#define Store(type, expr) (type.store(expr, std::memory_order_relaxed))
#define KeepSearchRunning(ec) (Store(ec.is_running, true))
#define StopSearch(ec) (Store(ec.is_running, false))

struct EngineController {
    std::atomic<bool>   is_running;

    /// TC Stuff ///
    std::atomic<int>      time        = -1;
    std::atomic<int>      max_nodes   = -1;
    std::atomic<int>      moves_to_go = -1;
    std::atomic<int>      max_depth   = -1;
    std::atomic<bool>     inf         = -1;
};

struct EngineSearchStuff {
    int pvLength[MAX_PLY];
    chess::Move pvTable[MAX_PLY][MAX_PLY];
    uint64_t nodes = 0;
    int checks = CHECK_RATE;
    int history[2][64][64] = {};

    chess::Move killerMoves[2][MAX_PLY];
};

constexpr void ResetHistory(EngineSearchStuff& ess) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 64; k++) {
                ess.history[i][j][k] = 0;
            }
        }
    }
}