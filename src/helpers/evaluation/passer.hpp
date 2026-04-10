#pragma once
#include "../../include/chess.hpp"

// fills are from the white side, so they fill up from the bottom to the top.

inline chess::Bitboard FrontFill(chess::Bitboard b) {
    b |= b << 8;
    b |= b << 16;
    b |= b << 32;
    return b;
}

inline chess::Bitboard BackFill(chess::Bitboard b) {
    b |= b >> 8;
    b |= b >> 16;
    b |= b >> 32;
    return b;
}

chess::Bitboard GeneratePasserMask(const chess::Board& board, chess::Color color, chess::Bitboard pawns);
chess::Bitboard GetAllPassers(const chess::Board& board, chess::Color color);