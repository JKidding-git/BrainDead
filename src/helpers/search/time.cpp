#include "time.hpp"

bool checkTime(bool iter, EngineController& ec, EngineSearchStuff& ess, clk t0) {
    if (!(Load(ec.is_running))) {
        StopSearch(ec);
        return true;
    }

    if (Load(ec.max_nodes) > 0 && ess.nodes >= Load(ec.max_nodes)) {
        StopSearch(ec);
        return true;
    }

    if (ess.checks > 0 && !iter) {
        ess.checks--;
        KeepSearchRunning(ec);
        return false;
    }

    ess.checks = CHECK_RATE;

    if (Load(ec.time) == 0) {
        KeepSearchRunning(ec);
        return false;
    }

    clk now = std::chrono::steady_clock::now();
    uint64_t timeNow = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();

    if (timeNow / 1'000'000 > Load(ec.time)) {
        StopSearch(ec);
        return true;
    }

    KeepSearchRunning(ec);
    return false;
}