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
static constexpr score piece_values[6] = {S(184, 149), S(490, 340), S(484, 392), S(641, 545), S(1269, 945), S(0, 0)};
static constexpr score passer_bonuses[8] = {S(0, 0), S(26, 26), S(7, 39), S(10, 93), S(61, 155), S(93, 300), S(183, 304), S(0, 0)};
static constexpr score isolated_pawn_penalty[9] = {S(0, 0), S(-33, -28), S(-60, -64), S(-106, -110), S(-148, -158), S(-154, -209), S(-161, -209), S(-349, -188), S(0, 0)};
static constexpr score backward_pawn_penalty[9] = {S(0, 0), S(-34, -18), S(-53, -17), S(-64, -22), S(-83, -21), S(-103, 0), S(-164, 40), S(126, -248), S(0, 0)};
static constexpr score king_pawn_shield_bonus[4] = {S(0, 0), S(57, -30), S(65, -18), S(58, -11)};
static constexpr score unsafe_square_penalty[28] = {S(0, 0), S(-10, -14), S(-19, -10), S(-23, -8), S(-25, -6), S(-29, -6), S(-30, -4), S(-33, -3), S(-39, -2), S(-39, -1), S(-41, -1), S(-47, 0), S(-49, 3), S(-51, 5), S(-58, 9), S(-58, 8), S(-59, 9), S(-49, 5), S(-33, 5), S(-77, 29), S(-97, 50), S(-76, -35), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
static constexpr score bishop_pair_bonus = S(86, 111);

// Mobility
static constexpr score mobility_knight[9] = {S(227, 161), S(271, 189), S(292, 236), S(300, 272), S(323, 281), S(333, 297), S(352, 303), S(374, 311), S(333, 268)};
static constexpr score mobility_bishop[14] = {S(271, 133), S(230, 168), S(255, 210), S(286, 236), S(311, 252), S(328, 271), S(341, 289), S(351, 299), S(357, 313), S(381, 316), S(414, 313), S(439, 317), S(393, 339), S(345, 271)};
static constexpr score mobility_rook[15] = {S(588, 125), S(377, 347), S(395, 364), S(399, 393), S(402, 437), S(413, 454), S(430, 476), S(440, 485), S(466, 490), S(485, 498), S(517, 501), S(531, 514), S(556, 520), S(578, 523), S(516, 452)};
static constexpr score mobility_queen[28] = {S(0, 0), S(1073, 157), S(1097, 152), S(860, 610), S(862, 521), S(867, 648), S(887, 743), S(894, 758), S(905, 752), S(918, 765), S(928, 816), S(933, 835), S(949, 862), S(960, 886), S(971, 888), S(972, 928), S(984, 935), S(985, 955), S(1002, 950), S(1039, 962), S(1039, 990), S(1053, 989), S(1102, 974), S(1080, 1009), S(1126, 979), S(1240, 911), S(928, 1083), S(1785, 448)};


score eval_mobility(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);

    chess::Bitboard knights = board.pieces(chess::PieceType::KNIGHT, color);
    chess::Bitboard bishops = board.pieces(chess::PieceType::BISHOP, color);
    chess::Bitboard rooks = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard queens = board.pieces(chess::PieceType::QUEEN, color);

    // Area
    chess::Bitboard occupied = board.occ();
    chess::Bitboard opp_pawn = board.pieces(chess::PieceType::PAWN, ~color);
    chess::Bitboard our_pawn = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard pawn_stater_rank = 0;
    chess::Bitboard pawn_attack_mask = 0;
    if (color == chess::Color::WHITE) {
        pawn_attack_mask |= chess::attacks::pawnLeftAttacks<chess::Color::BLACK>(opp_pawn);
        pawn_attack_mask |= chess::attacks::pawnRightAttacks<chess::Color::BLACK>(opp_pawn);

        pawn_stater_rank = chess::Bitboard(chess::Rank::RANK_2);
    } else {
        pawn_attack_mask |= chess::attacks::pawnLeftAttacks<chess::Color::WHITE>(opp_pawn);
        pawn_attack_mask |= chess::attacks::pawnRightAttacks<chess::Color::WHITE>(opp_pawn);

        pawn_stater_rank = chess::Bitboard(chess::Rank::RANK_7);
    }

    our_pawn &= pawn_stater_rank;
    our_pawn = our_pawn << 8;

    // Mobility itself
    while (knights) {
        chess::Square sq = knights.pop();
        int mobility = ((chess::attacks::knight(sq) & ~pawn_attack_mask) & ~our_pawn).count();

        #ifdef TUNE
            trace.mobility_knight[mobility][static_cast<bool>(color)]++;
        #endif

        value += mobility_knight[mobility];
    }
    
    while (bishops) {
        chess::Square sq = bishops.pop();
        int mobility = ((chess::attacks::bishop(sq, occupied) & ~pawn_attack_mask) & ~our_pawn).count();

        #ifdef TUNE
            trace.mobility_bishop[mobility][static_cast<bool>(color)]++;
        #endif

        value += mobility_bishop[mobility];
    }

    while (rooks) {
        chess::Square sq = rooks.pop();
        int mobility = ((chess::attacks::rook(sq, occupied) & ~pawn_attack_mask) & ~our_pawn).count();

        #ifdef TUNE
            trace.mobility_rook[mobility][static_cast<bool>(color)]++;
        #endif

        value += mobility_rook[mobility];
    }

    while (queens) {
        chess::Square sq = queens.pop();
        int mobility = ((chess::attacks::queen(sq, occupied) & ~pawn_attack_mask) & ~our_pawn).count();

        #ifdef TUNE
            trace.mobility_queen[mobility][static_cast<bool>(color)]++;
        #endif

        value += mobility_queen[mobility];
    }

    return value;
}

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
        trace.unsafe_square_penalty[count][static_cast<bool>(color)] += count;
    #endif

    value += unsafe_square_penalty[count];

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
    value += eval_mobility(board, chess::Color::WHITE, trace) - eval_mobility(board, chess::Color::BLACK, trace);

    return value;
}

int evaluate(const chess::Board& board, Trace& trace) {
    bool stm = board.sideToMove() == chess::Color::WHITE;

    score value = eval_colors(board, trace);
    int out = Blend(board, value);

    return out * (stm ? 1 : -1);
}