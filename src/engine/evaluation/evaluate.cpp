#include "evaluate.hpp"
#include "../../helpers/evaluation/score.hpp"
#include "../../helpers/evaluation/psqt.hpp"

static constexpr score piece_values[6] = {S(100, 100), S(325, 325), S(350, 350), S(500, 500), S(900, 900), S(0, 0)};

static constexpr chess::PieceType pts[6] = {chess::PieceType::PAWN, chess::PieceType::KNIGHT, chess::PieceType::BISHOP, chess::PieceType::ROOK, chess::PieceType::QUEEN, chess::PieceType::KING};

score eval_piece(const chess::Board& board, const chess::Color color) {
    score v = S(0, 0);
    for (auto piece_types : pts) {
        chess::Bitboard pieces = board.pieces(piece_types, color);
        int p_index = int(piece_types);
        int count = pieces.count();

        v += piece_values[p_index] * count;
    }

    return v;
}

score psqt_eval(const chess::Board& board, const chess::Color color) {
    score v = S(0, 0);
    chess::Bitboard pieces = board.us(color);
    

    while (pieces) {
        uint8_t sq = pieces.pop();
        chess::PieceType pt = board.at(sq).type();
        int p_index = int(pt);

        if (color == chess::Color::WHITE) {
            v += psqt[p_index][sq^56];
        } else {
            v += psqt[p_index][sq];
        }
    }

    return v;
}

score eval_colors(const chess::Board& board) {
    score v = S(0, 0);

    v += eval_piece(board, chess::Color::WHITE) - eval_piece(board,chess::Color::BLACK);
    v += psqt_eval(board, chess::Color::WHITE) - psqt_eval(board, chess::Color::BLACK);

    return v;
}

int evaluate(const chess::Board& board) {
    bool stm = board.sideToMove() == chess::Color::WHITE;

    score v = eval_colors(board);
    int out = Blend(board, v);

    return out * (stm ? 1 : -1);
}