#pragma once

#include <cstdint>
#include <thread>



struct UCIController {
    bool debug = false;
    
};

struct UCIGoTimeController {
    int64_t nodes = -1;
    int8_t depth = -1;

    int moves_to_go = -1;
    int move_time = -1;

    int wtime = -1;
    int winc = -1;

    int btime = -1;
    int binc = -1;

    void reset() {
        nodes = -1;
        depth = -1;

        moves_to_go = -1;
        move_time = -1;
        wtime = -1;
        winc = -1;

        btime = -1;
        binc = -1;
    }
};