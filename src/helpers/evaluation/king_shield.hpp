#pragma once
#include "../../include/chess.hpp"

chess::Bitboard GenerateKingShieldMask(const chess::Bitboard& king_sq_bit, chess::Color color);

chess::Bitboard GetCastlingSquares(chess::Color color);