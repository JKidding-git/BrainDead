#include "evaluate.hpp"
#include "../../helpers/evaluation/score.hpp"
#include "../../helpers/evaluation/psqt.hpp"
#include "../../helpers/evaluation/mobility.hpp"

static constexpr score piece_values[6] = {S(100, 100), S(325, 325), S(350, 350), S(500, 500), S(900, 900), S(0, 0)};
static constexpr chess::PieceType pts[6] = {chess::PieceType::PAWN, chess::PieceType::KNIGHT, chess::PieceType::BISHOP, chess::PieceType::ROOK, chess::PieceType::QUEEN, chess::PieceType::KING};


static constexpr chess::Bitboard file_a(0x0101010101010101ULL);
static constexpr chess::Bitboard file_h(0x8080808080808080ULL);


score eval_mobility(const chess::Board& board, const chess::Color color) {
    score value = S(0, 0);

    chess::Bitboard pawns = board.pieces(chess::PieceType::PAWN, color);
    chess::Bitboard occupied = board.occ();
    chess::Bitboard enemy_pawns = board.pieces(chess::PieceType::PAWN, ~color);

    
    const chess::Bitboard start_rank = (color == chess::Color::WHITE)
        ? chess::Bitboard(0x000000000000FF00ULL)
        : chess::Bitboard(0x00FF000000000000ULL);

    chess::Bitboard blocked = pawns & (
        start_rank |
        (color == chess::Color::WHITE ? (occupied >> 8) : (occupied << 8))
    );

    chess::Bitboard enemy_pawn_attacks = (color == chess::Color::WHITE)
        ? (((enemy_pawns >> 7) & ~file_a) | ((enemy_pawns >> 9) & ~file_h))
        : (((enemy_pawns << 7) & ~file_h) | ((enemy_pawns << 9) & ~file_a));

    chess::Bitboard mobility_area = ~(blocked | enemy_pawn_attacks);

    // Mobility for knights, bishops, rooks, queens
    chess::Bitboard knights = board.pieces(chess::PieceType::KNIGHT, color);
    chess::Bitboard bishops = board.pieces(chess::PieceType::BISHOP, color);
    chess::Bitboard rooks   = board.pieces(chess::PieceType::ROOK, color);
    chess::Bitboard queens  = board.pieces(chess::PieceType::QUEEN, color);

    // indeces for each piece type - 1.
    constexpr int knight_index = static_cast<int>(chess::PieceType::KNIGHT) - 1;
    constexpr int bishop_index = static_cast<int>(chess::PieceType::BISHOP) - 1;
    constexpr int rook_index   = static_cast<int>(chess::PieceType::ROOK)   - 1;
    constexpr int queen_index  = static_cast<int>(chess::PieceType::QUEEN)  - 1;

    // attacks count for each piece type
    while (knights) {
        chess::Square sq = knights.pop();
        chess::Bitboard attacks = chess::attacks::knight(sq);
        attacks &= mobility_area;
        value += mobility_scores[knight_index][attacks.count()];
    }

    while (bishops) {
        chess::Square sq = bishops.pop();
        chess::Bitboard attacks = chess::attacks::bishop(sq, occupied);
        attacks &= mobility_area;
        value += mobility_scores[bishop_index][attacks.count()];
    }

    while (rooks) {
        chess::Square sq = rooks.pop();
        chess::Bitboard attacks = chess::attacks::rook(sq, occupied);
        attacks &= mobility_area;
        value += mobility_scores[rook_index][attacks.count()];
    }

    while (queens) {
        chess::Square sq = queens.pop();
        chess::Bitboard attacks = chess::attacks::queen(sq, occupied);
        attacks &= mobility_area;
        value += mobility_scores[queen_index][attacks.count()];
    }

    return value;
}

score eval_piece(const chess::Board& board, const chess::Color color) {
    score value = S(0, 0);
    for (auto piece_types : pts) {
        chess::Bitboard pieces = board.pieces(piece_types, color);
        int p_index = int(piece_types);
        int count = pieces.count();

        value += piece_values[p_index] * count;
    }

    return value;
}

score psqt_eval(const chess::Board& board, const chess::Color color) {
    score value = S(0, 0);
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
    value += eval_mobility(board, chess::Color::WHITE) - eval_mobility(board, chess::Color::BLACK);

    return value;
}

int evaluate(const chess::Board& board) {
    bool stm = board.sideToMove() == chess::Color::WHITE;

    score value = eval_colors(board);
    int out = Blend(board, value);

    return out * (stm ? 1 : -1);
}