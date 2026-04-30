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
static constexpr score piece_values[6] = {S(201, 183), S(609, 503), S(632, 532), S(846, 783), S(1605, 1401), S(0, 0)};
static constexpr score passer_bonuses[8] = {S(0, 0), S(24, 18), S(9, 29), S(15, 78), S(60, 133), S(92, 259), S(201, 303), S(0, 0)};
static constexpr score isolated_pawn_penalty[9] = {S(0, 0), S(-29, -23), S(-54, -52), S(-97, -90), S(-138, -127), S(-145, -159), S(-149, -144), S(-274, -114), S(-75, -75)};
static constexpr score backward_pawn_penalty[9] = {S(0, 0), S(-30, -16), S(-46, -15), S(-56, -15), S(-72, -11), S(-89, 9), S(-140, 45), S(53, -132), S(-10, -15)};
static constexpr score king_pawn_shield_bonus[4] = {S(26, -73), S(72, -90), S(124, -103), S(147, -118)};
static constexpr score unsafe_square_penalty[28] = {S(-10, -5), S(-10, -13), S(-26, -13), S(-44, -13), S(-63, -9), S(-90, -9), S(-115, -4), S(-145, 1), S(-208, 14), S(-234, 18), S(-273, 21), S(-353, 46), S(-406, 80), S(-467, 113), S(-576, 165), S(-615, 169), S(-677, 186), S(-568, 143), S(-312, 131), S(-621, 274), S(-157, 69), S(-499, -140), S(-10, -5), S(-10, -5), S(-10, -5), S(-10, -5), S(-10, -5), S(-10, -5)};
static constexpr score bishop_pair_bonus = S(77, 97);
static constexpr score mobility_knight[9] = {S(117, 52), S(155, 79), S(173, 120), S(181, 152), S(201, 160), S(211, 174), S(228, 180), S(247, 188), S(206, 147)};
static constexpr score mobility_bishop[14] = {S(147, 45), S(113, 73), S(134, 109), S(161, 133), S(183, 146), S(198, 163), S(210, 179), S(219, 188), S(226, 200), S(247, 204), S(276, 202), S(297, 206), S(259, 225), S(208, 159)};
static constexpr score mobility_rook[15] = {S(374, 5), S(201, 180), S(216, 196), S(221, 221), S(224, 259), S(235, 274), S(249, 294), S(259, 302), S(281, 307), S(298, 314), S(326, 318), S(339, 330), S(362, 335), S(380, 339), S(316, 268)};
static constexpr score mobility_queen[28] = {S(0, 0), S(667, 49), S(740, 69), S(555, 298), S(558, 223), S(563, 333), S(581, 413), S(588, 427), S(598, 423), S(610, 435), S(619, 479), S(625, 495), S(638, 519), S(649, 540), S(659, 542), S(661, 576), S(672, 583), S(674, 601), S(689, 597), S(720, 608), S(722, 632), S(735, 632), S(775, 622), S(758, 651), S(798, 627), S(894, 571), S(624, 721), S(1108, 276)};
static constexpr score stacked_pawns[9] = {S(0, 0), S(0, 0), S(-11, -43), S(-7, -157), S(-55, -71), S(-84, -104), S(-116, -140), S(-152, -180), S(-192, -224)};


score stacked_pawn_eval(const chess::Board& board, chess::Color color, Trace& trace) {
    score value = S(0, 0);
    chess::Bitboard pawns = board.pieces(chess::PieceType::PAWN, color);

    for (int file = 0; file < 8; file++) {
        int count = (pawns & chess::Bitboard(chess::File(file))).count();

        if (count <= 1) continue;

        #ifdef TUNE
            trace.stacked_pawns[count][static_cast<bool>(color)]++;
        #endif

        value += stacked_pawns[count];
    }

    return value;
}

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

    if (color == chess::Color::WHITE) our_pawn = our_pawn << 8;
    else our_pawn >> 8;

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
        trace.unsafe_square_penalty[count][static_cast<bool>(color)]++;
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
    value += stacked_pawn_eval(board, chess::Color::WHITE, trace) - stacked_pawn_eval(board, chess::Color::BLACK, trace);

    return value;
}

int evaluate(const chess::Board& board, Trace& trace) {
    bool stm = board.sideToMove() == chess::Color::WHITE;

    score value = eval_colors(board, trace);
    int out = Blend(board, value);

    return out * (stm ? 1 : -1);
}