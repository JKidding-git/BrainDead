#pragma once
#include "../../include/chess.hpp"

chess::Bitboard GetLeastValuablePieceSQ(const chess::Board& board, chess::Color color, chess::Bitboard attacks_bits, chess::PieceType &piece);
int SEE(const chess::Board& board, chess::Square from, chess::Square to, chess::PieceType target, chess::PieceType a_piece, chess::Color color);

