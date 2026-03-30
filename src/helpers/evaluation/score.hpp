#pragma once
#include <cstdint>
#include "../../include/chess.hpp"

struct score {
    int32_t middlegame;
    int32_t endgame;
};

/// @brief Constructs a score value with separate middlegame and endgame components.
/// @param middlegame The score evaluation for the middlegame phase.
/// @param endgame The score evaluation for the endgame phase.
/// @return A score object containing both middlegame and endgame values.
inline constexpr score S(const int32_t middlegame, const int32_t endgame) {return {middlegame, endgame};}

int32_t Blend(const chess::Board& board, score& value);

// Others, please ignore.

inline constexpr score& operator+=(score& a, score b) {
    a.middlegame += b.middlegame;
    a.endgame    += b.endgame;
    return a;
}

inline constexpr score& operator+=(score& a, int32_t k) {
    a.middlegame += k;
    a.endgame    += k;
    return a;
}

inline constexpr score& operator-=(score& a, score b) {
    a.middlegame -= b.middlegame;
    a.endgame    -= b.endgame;
    return a;
}

inline constexpr score& operator-=(score& a, int32_t k) {
    a.middlegame -= k;
    a.endgame    -= k;
    return a;
}

inline constexpr score& operator*=(score& a, score b) {
    a.middlegame *= b.middlegame;
    a.endgame    *= b.endgame;
    return a;
}

inline constexpr score& operator*=(score& a, int32_t k) {
    a.middlegame *= k;
    a.endgame    *= k;
    return a;
}

inline constexpr score operator+(score a, score b) {
    return {a.middlegame + b.middlegame, a.endgame + b.endgame};
}

inline constexpr score operator+(score a, int32_t k) {
    return {a.middlegame + k, a.endgame + k};
}

inline constexpr score operator+(int32_t k, score a) {
    return a + k;
}

inline constexpr score operator-(score a, score b) {
    return {a.middlegame - b.middlegame, a.endgame - b.endgame};
}

inline constexpr score operator-(score a, int32_t k) {
    return {a.middlegame - k, a.endgame - k};
}

inline constexpr score operator-(int32_t k, score a) {
    return {k - a.middlegame, k - a.endgame};
}

inline constexpr score operator*(score a, score b) {
    return {a.middlegame * b.middlegame, a.endgame * b.endgame};
}

inline constexpr score operator*(score a, int32_t k) {
    return {a.middlegame * k, a.endgame * k};
}

inline constexpr score operator*(int32_t k, score a) {
    return a * k;
}