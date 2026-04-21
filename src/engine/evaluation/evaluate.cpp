#include "evaluate.hpp"
#include "../../helpers/evaluation/score.hpp"
#include "../../helpers/evaluation/psqt.hpp"
#include "../../helpers/evaluation/passer.hpp"
#include "../../helpers/evaluation/isolated.hpp"
#include "../../helpers/evaluation/backward.hpp"
#include "../../helpers/evaluation/king_shield.hpp"
#include "../../helpers/evaluation/unsafe_squares.hpp"


static constexpr chess::PieceType pts[6] = {chess::PieceType::PAWN, chess::PieceType::KNIGHT, chess::PieceType::BISHOP, chess::PieceType::ROOK, chess::PieceType::QUEEN, chess::PieceType::KING};

// these values are now tuned
// yayyyy!
static constexpr score piece_values[6] = {S(205, 181), S(707, 582), S(736, 641), S(971, 945), S(1808, 1712), S(0, 0)};
static constexpr score passer_bonuses[8] = {S(0, 0), S(28, 26), S(12, 35), S(15, 82), S(59, 138), S(97, 264), S(215, 301), S(0, 0)};
static constexpr score isolated_pawn_penalty[9] = {S(0, 0), S(-27, -26), S(-51, -60), S(-93, -103), S(-133, -146), S(-141, -190), S(-150, -189), S(-199, -204), S(-75, -75)};
static constexpr score backward_pawn_penalty[9] = {S(0, 0), S(-34, -16), S(-50, -15), S(-59, -20), S(-74, -18), S(-93, -1), S(-131, 28), S(9, -168), S(-10, -15)};
static constexpr score king_pawn_shield_bonus[4] = {S(0, 0), S(47, -23), S(53, -12), S(48, -5)};
static constexpr score unsafe_square_penalty = S(-32, -3);
static constexpr score bishop_pair_bonus = S(79, 90);



score eval_bishop_pair(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);
    chess::Bitboard bishops = board.pieces(chess::PieceType::BISHOP, color);

    #ifdef TUNE
        if (bishops.count() >= 2) trace.bishop_pair_bonus[static_cast<bool>(color)]++;
    #endif

    if (bishops.count() >= 2) value += bishop_pair_bonus;

    return value;
}

score eval_unsafe_squares(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);
    chess::Bitboard unsafe_squares = GetAllKingUnsafeSquares(board, color);

    // No unsafe squares, no penalty
    if (!unsafe_squares.getBits()) return value;

    int count = unsafe_squares.count();

    #ifdef TUNE
        trace.unsafe_square_penalty[static_cast<bool>(color)] += count;
    #endif

    value += unsafe_square_penalty * count;

    return value;
}

score eval_king_shield(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);
    chess::Square king_sq = board.kingSq(color);
    chess::Bitboard king_sq_bit = chess::Bitboard::fromSquare(king_sq);
    chess::Bitboard castling_squares = GetCastlingSquares(color);
    chess::Bitboard pawns = board.pieces(chess::PieceType::PAWN, color);

    // If the king is not castled, skip the king shield evaluation, since it's not relevant
    if (!(castling_squares & king_sq_bit)) return value;
    if (!pawns.getBits()) return value;

    chess::Bitboard shield_mask = GenerateKingShieldMask(king_sq_bit, color);
    int count = (shield_mask & pawns).count();
    int pawns_should_have_at_most_three = std::min(3, count);

    #ifdef TUNE
        trace.king_pawn_shield_bonus[pawns_should_have_at_most_three][static_cast<bool>(color)]++;
    #endif

    value += king_pawn_shield_bonus[pawns_should_have_at_most_three];

    return value;
}

score eval_backward_pawns(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);
    chess::Bitboard backward_pawns = GetAllBackwardPawns(board, color);

    // No backward pawns, no penalty
    if (!backward_pawns.getBits()) return value;

    int count = backward_pawns.count();

    #ifdef TUNE
        trace.backward_pawn_penalty[count][static_cast<bool>(color)]++;
    #endif

    value += backward_pawn_penalty[count];
    return value;
}

score eval_isolated_pawns(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);
    chess::Bitboard isolated_pawns = GetAllIsolatedPawns(board, color);

    // No isolated pawns, no penalty
    if (!isolated_pawns.getBits()) return value;

    int count = isolated_pawns.count();

    #ifdef TUNE
        trace.isolated_pawn_penalty[count][static_cast<bool>(color)]++;
    #endif

    value += isolated_pawn_penalty[count];

    return value;
}

score eval_passers(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);
    chess::Bitboard passers = GetAllPassers(board, color);

    // No passers, no bonus
    if (!passers.getBits()) return value;

    int rank_reduction = 0;
    if (color == chess::Color::BLACK) rank_reduction = 7;
    
    while (passers) {
        chess::Square sq = passers.pop();
        int rank = sq.rank();

        #ifdef TUNE
            trace.passer_bonuses[rank ^ rank_reduction][static_cast<bool>(color)]++;
        #endif

        value += passer_bonuses[rank ^ rank_reduction];
    }

    return value;
}

score eval_piece(const chess::Board& board, const chess::Color color, Trace& trace) {
    score value = S(0, 0);
    for (auto piece_types : pts) {
        chess::Bitboard pieces = board.pieces(piece_types, color);

        // skip if no pieces of this type
        if (!pieces.getBits()) continue;

        int p_index = int(piece_types);
        int count = pieces.count();

        #ifdef TUNE
            trace.piece_values[p_index][static_cast<bool>(color)] += count;
        #endif

        value += piece_values[p_index] * count;
    }

    return value;
}

score psqt_eval(const chess::Board& board, const chess::Color color, Trace& trace) {
    score value = S(0, 0);

    // Impossible to skip, since we need to have at least two kings
    // to have a legal position, and the king's psqt is the most important one, so we can't skip it
    chess::Bitboard pieces = board.us(color);

    while (pieces) {
        chess::Square sq = pieces.pop();
        chess::PieceType pt = board.at(sq).type();
        int p_index = int(pt);

        int square_relative = sq.relative_square(color).index();

        #ifdef TUNE
            trace.psqt[p_index][square_relative][static_cast<bool>(color)]++;
        #endif

        value += psqt[p_index][square_relative];
    }

    return value;
}


score eval_king_safety(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);

    value += eval_unsafe_squares(board, color, trace);
    value += eval_king_shield(board, color, trace);

    return value;
}

score eval_pawn_structure(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);

    value += eval_passers(board, color, trace);
    value += eval_isolated_pawns(board, color, trace);
    value += eval_backward_pawns(board, color, trace);

    return value;
}

score eval_colors(const chess::Board& board, Trace& trace) {
    score value = S(0, 0);

    value += eval_piece(board, chess::Color::WHITE, trace) - eval_piece(board, chess::Color::BLACK, trace);
    value += psqt_eval(board, chess::Color::WHITE, trace) - psqt_eval(board, chess::Color::BLACK, trace);
    value += eval_bishop_pair(board, chess::Color::WHITE, trace) - eval_bishop_pair(board, chess::Color::BLACK, trace);

    value += eval_king_safety(board, chess::Color::WHITE, trace) - eval_king_safety(board, chess::Color::BLACK, trace);
    value += eval_pawn_structure(board, chess::Color::WHITE, trace) - eval_pawn_structure(board, chess::Color::BLACK, trace);

    return value;
}

int evaluate(const chess::Board& board, Trace& trace) {
    bool stm = board.sideToMove() == chess::Color::WHITE;

    score value = eval_colors(board, trace);
    int out = Blend(board, value);

    return out * (stm ? 1 : -1);
}