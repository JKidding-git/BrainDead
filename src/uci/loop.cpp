#include "loop.hpp"

#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <string>
#include <cstdint>
#include <cassert>
#include <chrono>
#include <unordered_map>
#include <thread>

#include "../include/chess.hpp"
#include "../helpers/tests.hpp"
#include "../helpers/misc.hpp"
#include "uci_helper.hpp"
#include "position.hpp"
#include "options/spin.hpp"
#include "options/check.hpp"

#include "../helpers/evaluation/score.hpp"
#include "../helpers/search/controller.hpp"

#include "../engine/search/search.hpp"

void UCICommandOutput() {
    FlushPrint("id name BrainDead\n");
    FlushPrint("id author JKidding\n\n");

    ShowAllSpinsForUCI(spin_entries);
    ShowAllChecksForUCI(check_entries);

    FlushPrint("uciok\n");
}



void Init(UCIGoTimeController& uci_go_time_controller) {
    uci_go_time_controller.reset();
}

int GetTime(UCIController& uci_controller, UCIGoTimeController& uci_go_time_controller, EngineController& engine_controller, chess::Board& board) {
    int mtg = 24;
    int move_overhead = spin_entries["move_overhead"].default_value;

    if (uci_go_time_controller.moves_to_go > 0) {
        mtg = uci_go_time_controller.moves_to_go + 2;
    }

    int time = (board.sideToMove() == chess::Color::WHITE) ? uci_go_time_controller.wtime : uci_go_time_controller.btime;
    int inc = (board.sideToMove() == chess::Color::WHITE) ? uci_go_time_controller.winc : uci_go_time_controller.binc;
    int result = 0;

    result += time / mtg;
    result += inc;

    if (result > move_overhead) result -= move_overhead;
    
    if (uci_controller.debug) FlushPrint("info string time_control = " + std::to_string(result) + '\n');

    return result;
}

void UCIGo(std::string& line, UCIController& uci_controller, UCIGoTimeController& uci_go_time_controller, EngineController& engine_controller, chess::Board& board, EngineSearchStuff& ess) {
    std::istringstream iss(line);
    std::string token;

    bool is_move_time = false;
    bool is_time_mentioned = false;
    bool is_infinite = false;

    while (iss >> token) {

        if (token == "infinite")  {
            is_infinite = true;
            break;
        }

        int64_t v = -1;
        if (token == "depth")          { iss >> v; uci_go_time_controller.depth       = v; }
        else if (token == "movetime")  { iss >> v; uci_go_time_controller.move_time   = v; is_move_time = true; }
        else if (token == "nodes")     { iss >> v; uci_go_time_controller.nodes       = v; }
        else if (token == "movestogo") { iss >> v; uci_go_time_controller.moves_to_go = v; }
        else if (token == "wtime")     { iss >> v; uci_go_time_controller.wtime       = v; is_time_mentioned = true; }
        else if (token == "winc")      { iss >> v; uci_go_time_controller.winc        = v; is_time_mentioned = true; }
        else if (token == "btime")     { iss >> v; uci_go_time_controller.btime       = v; is_time_mentioned = true; }
        else if (token == "binc")      { iss >> v; uci_go_time_controller.binc        = v; is_time_mentioned = true; }
        
    }

    if (uci_controller.debug) {
        std::string inf_debug = is_infinite ? "true\n" : "false\n";
        FlushPrint("info string depth = "       + std::to_string(uci_go_time_controller.depth)       + '\n');
        FlushPrint("info string move_time = "   + std::to_string(uci_go_time_controller.move_time)   + '\n');
        FlushPrint("info string nodes = "       + std::to_string(uci_go_time_controller.nodes)       + '\n');
        FlushPrint("info string moves_to_go = " + std::to_string(uci_go_time_controller.moves_to_go) + '\n');
        FlushPrint("info string wtime = "       + std::to_string(uci_go_time_controller.wtime)       + '\n');
        FlushPrint("info string winc = "        + std::to_string(uci_go_time_controller.winc)        + '\n');
        FlushPrint("info string btime = "       + std::to_string(uci_go_time_controller.btime)       + '\n');
        FlushPrint("info string binc = "        + std::to_string(uci_go_time_controller.binc)        + '\n');
        FlushPrint("info string infinite = " + inf_debug);
    }

    // Reset per-search limits so previous go commands cannot leak state.
    Store(engine_controller.inf, false);
    Store(engine_controller.time, 0);
    Store(engine_controller.max_nodes, -1);
    Store(engine_controller.moves_to_go, -1);
    Store(engine_controller.max_depth, MAX_PLY);

    if (uci_go_time_controller.depth > 0) {
        Store(engine_controller.max_depth, uci_go_time_controller.depth);
    }

    if (uci_go_time_controller.nodes > 0) {
        Store(engine_controller.max_nodes, static_cast<int>(uci_go_time_controller.nodes));
    }

    if (uci_go_time_controller.moves_to_go > 0) {
        Store(engine_controller.moves_to_go, uci_go_time_controller.moves_to_go);
    }

    if (is_infinite) {
        Store(engine_controller.inf, true);
    } else if (is_move_time) {
        Store(engine_controller.time, uci_go_time_controller.move_time);
    } else if (is_time_mentioned) {
        int result = GetTime(uci_controller, uci_go_time_controller, engine_controller, board);
        Store(engine_controller.time, result);
    }

    if (search.joinable()) search.join();
    search = std::thread(IterativeDeepening, std::ref(board), std::ref(ess), std::ref(engine_controller));
    
}

constexpr inline bool FirstWordEqualTo(const words_vector& word, const std::string& token) {return word[0] == token;}

void UCILoop() {
    std::string line;
    UCIController uci_controller;
    UCIGoTimeController uci_go_time_controller;
    chess::Board board;

    EngineController engine_controller;
    EngineSearchStuff ess;


    AddSpin("move_overhead", 50, 0, 5000, spin_entries);

    FlushPrint("stuff is initialized\n");
    while(std::getline(std::cin, line)) {

        // If debug is true, print the UCI command
        if (uci_controller.debug) FlushPrint("info string UCI command got: \""+ line + "\"\n");

        if (line.empty()) continue;
        
        words_vector word = SplitWords(line);
        if (word.empty()) continue;

        if (FirstWordEqualTo(word, "quit")) {
            StopSearch(engine_controller);
            if (search.joinable()) search.join();
            break;
        } else if (FirstWordEqualTo(word, "uci")) {
            UCICommandOutput();
        } else if (FirstWordEqualTo(word, "setoption") && word.size() >= 4 && word[1] == "name" && word[3] == "value") {
            SetOptionSpin(word, spin_entries);
            SetOptionCheck(word, check_entries);
        } else if (FirstWordEqualTo(word, "debug")) {
            if (word.size() > 1) {
                bool result = word[1] == "on";
                uci_controller.debug = result;
            }
        } else if (FirstWordEqualTo(word, "isready")) {
            FlushPrint("readyok\n");
        } else if (FirstWordEqualTo(word, "ucinewgame")) {
            board.setFen(chess::constants::STARTPOS);
            Init(uci_go_time_controller);
        } else if (FirstWordEqualTo(word, "position")) {
            UCIPosition(word, uci_controller, board);
        } else if (FirstWordEqualTo(word, "go")) {
            Init(uci_go_time_controller);
            KeepSearchRunning(engine_controller);
            UCIGo(line, uci_controller, uci_go_time_controller, engine_controller, board, ess);
        } else if (FirstWordEqualTo(word, "stop")) {
            StopSearch(engine_controller);
            if (search.joinable()) search.join();
        }

        // OTHER COMMANDS
        if (FirstWordEqualTo(word, "test")) {
            RunTests();
            PerfTestAddSpin();
            PerfTestAddCheck();
        } else if(FirstWordEqualTo(word, "p")) {
            std::cout << board << std::endl;
        }

    }
}
