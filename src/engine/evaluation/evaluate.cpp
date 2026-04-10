#include "evaluate.hpp"
#include "../../helpers/evaluation/score.hpp"
#include "../../helpers/evaluation/psqt.hpp"
#include "../../helpers/evaluation/passer.hpp"

static constexpr chess::PieceType pts[6] = {chess::PieceType::PAWN, chess::PieceType::KNIGHT, chess::PieceType::BISHOP, chess::PieceType::ROOK, chess::PieceType::QUEEN, chess::PieceType::KING};

// Untuned values.
static constexpr score piece_values[6] = {S(100, 100), S(325, 325), S(350, 350), S(500, 500), S(900, 900), S(0, 0)};
static constexpr score passer_bonuses[8] = {S(0, 0), S(15, 15), S(15, 15), S(30, 30), S(50, 50), S(80, 80), S(120, 120), S(0, 0)};

score eval_passers(const chess::Board& board, chess::Color color) {
    score value = S(0, 0);
    chess::Bitboard passers = GetAllPassers(board, color);

    // No passers, no bonus
    if (!passers.getBits()) return value;

    int rank_reduction = 0;
    if (color == chess::Color::BLACK) rank_reduction = 7;
    
    while (passers) {
        chess::Square sq = passers.pop();
        int rank = sq.rank();

        value += passer_bonuses[rank ^ rank_reduction];
    }

    return value;
}

score eval_piece(const chess::Board& board, const chess::Color color) {
    score value = S(0, 0);
    for (auto piece_types : pts) {
        chess::Bitboard pieces = board.pieces(piece_types, color);

        // skip if no pieces of this type
        if (!pieces.getBits()) continue;

        int p_index = int(piece_types);
        int count = pieces.count();

        value += piece_values[p_index] * count;
    }

    return value;
}

score psqt_eval(const chess::Board& board, const chess::Color color) {
    score value = S(0, 0);

    // Impossible to skip, since we need to have at least two kings
    // to have a legal position, and the king's psqt is the most important one, so we can't skip it
    chess::Bitboard pieces = board.us(color);

    while (pieces) {
        uint8_t sq = pieces.pop();
        chess::PieceType pt = board.at(sq).type();
        int p_index = int(pt);

        if (color == chess::Color::WHITE) {
            value += psqt[p_index][sq^56];
        } else {
            value += psqt[p_index][sq];
        }
    }

    return value;
}

score eval_colors(const chess::Board& board) {
    score value = S(0, 0);

    value += eval_piece(board, chess::Color::WHITE) - eval_piece(board,chess::Color::BLACK);
    value += psqt_eval(board, chess::Color::WHITE) - psqt_eval(board, chess::Color::BLACK);
    value += eval_passers(board, chess::Color::WHITE) - eval_passers(board, chess::Color::BLACK);

    return value;
}

int evaluate(const chess::Board& board) {
    bool stm = board.sideToMove() == chess::Color::WHITE;

    score value = eval_colors(board);
    int out = Blend(board, value);

    return out * (stm ? 1 : -1);
}