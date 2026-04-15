#include "values.hpp"
#include <cmath>
#include "pv.hpp"



std::string convert_score(int score) {
    if (score >= VALUE_MATE_IN_PLY) {
        return "mate " + std::to_string(((VALUE_MATE - score) / 2) + ((VALUE_MATE - score) & 1));
    } else if (score <= VALUE_MATED_IN_PLY) {
        return "mate " + std::to_string(-((VALUE_MATE + score) / 2) + ((VALUE_MATE + score) & 1));
    } else {
        return "cp " + std::to_string(score);
    }
}

void stats(int depth, int score, uint64_t time, EngineSearchStuff& ess) {
    int time_in_ms = int(time / 1'000'000);
    int time_in_seconds = std::max(1, time_in_ms / 1'000);
    std::cout << "info depth "
              << depth
              << " score "
              << (convert_score(score))
              << " nodes "
              << std::to_string(ess.nodes)
              << " nps "
              << std::to_string(int(ess.nodes / time_in_seconds))
              << " time "
              << std::to_string(time_in_ms)
              << " pv"
              << getPV(ess)
              << '\n'
              << std::flush;
}