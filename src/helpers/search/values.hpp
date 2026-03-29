#pragma once
#include <cstdint>
#include <cassert>

struct EngineSearchStuff;

inline static constexpr uint8_t MAX_PLY = 60; // 30 full moves (60/2)
inline static constexpr int16_t CHECK_RATE = 256;
inline static constexpr int VALUE_INFINITE = 32001;
inline static constexpr int VALUE_NONE = 32002;
inline static constexpr int VALUE_MATE = 32000;
inline static constexpr int VALUE_MATE_IN_PLY = VALUE_MATE - MAX_PLY;
inline static constexpr int VALUE_MATED_IN_PLY = -VALUE_MATE_IN_PLY;

inline static constexpr int VALUE_TB_WIN = VALUE_MATE_IN_PLY;
inline static constexpr int VALUE_TB_LOSS = -VALUE_TB_WIN;
inline static constexpr int VALUE_TB_WIN_IN_MAX_PLY = VALUE_TB_WIN - MAX_PLY;
inline static constexpr int VALUE_TB_LOSS_IN_MAX_PLY = -VALUE_TB_WIN_IN_MAX_PLY;

void stats(int depth, int score, uint64_t time, EngineSearchStuff& ess);

inline int mate_in(int ply) {return (VALUE_MATE - ply);}
inline int mated_in(int ply) {return (ply - VALUE_MATE);}
inline constexpr bool is_valid(int value) { return value != VALUE_NONE; }

inline constexpr bool is_win(int value) {
    assert(is_valid(value));
    return value >= VALUE_TB_WIN_IN_MAX_PLY;
}

inline constexpr bool is_loss(int value) {
    assert(is_valid(value));
    return value <= VALUE_TB_LOSS_IN_MAX_PLY;
}