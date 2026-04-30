#include "search.hpp"
#include "../../helpers/misc.hpp"
#include "../../helpers/search/heuristics.hpp"
#include "move_picker.hpp"


// bonus_squared[depth-1]
static constexpr int bonus_squared[MAX_PLY] = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225, 256, 289, 324, 361, 400, 441, 484, 529, 576, 625, 676, 729, 784, 841, 900, 961, 1024, 1089, 1156, 1225, 1296, 1369, 1444, 1521, 1600, 1681, 1764, 1849, 1936, 2025, 2116, 2209, 2304, 2401, 2500, 2601, 2704, 2809, 2916, 3025, 3136, 3249, 3364, 3481, 3600};



int qSearch(chess::Board& board, int alpha, int beta, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0, Trace& trace) {
    if (!(Load(ec.is_running)) || checkTime(false, ec, ess, t0)) return 0;
    if (ply >= MAX_PLY) return evaluate(board, trace);

    int32_t bestValue = evaluate(board, trace);
    if (bestValue >= beta) return bestValue;
    if (bestValue > alpha) alpha = bestValue;

    ess.pvLength[ply] = ply;

    chess::Movelist moves;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, board);
    ScoreMoves<true>(board, moves, ess, ply, chess::Move::NULL_MOVE);


    for (int i = 0; i < moves.size(); i++) {
        ess.nodes++;

        PickMove(moves, i);
        chess::Move move = moves[i];

        board.makeMove(move);
        int score = -qSearch(board, -beta, -alpha, ply + 1, ess, ec, t0, trace);
        board.unmakeMove(move);

        if (score > bestValue) bestValue = score;

        if (score > alpha) {
            alpha = score;
            AddPV(move, ess, ply);
        }

        if (alpha >= beta) break;
    }

    return bestValue;
}

int alphaBeta(chess::Board& board, int alpha, int beta, int depth, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0, Trace& trace, bool shouldNull) {
    bool time = checkTime(false, ec, ess, t0);
    if (time) return 0;
    if (ply >= MAX_PLY) return evaluate(board, trace);

    
    ess.pvLength[ply] = ply;
    bool NotRootNode = ply > 0;
    uint64_t hash = board.hash();
    bool inCheck = board.inCheck();
    chess::Color side = board.sideToMove();
    int score = 0;
    
    // just a little optimization trick, hopefully it works.
    tt.Prefetch(hash);

    if (NotRootNode) {
        if (board.isRepetition(1))  return -5;
        if (board.isHalfMoveDraw()) return 0;

        // Mate Distance Pruning
        alpha = std::max(alpha, mated_in(ply));
        beta = std::min(beta, mate_in(ply + 1));
        if (alpha >= beta) return alpha;
    }

    if (depth <= 0) return qSearch(board, alpha, beta, ply, ess, ec, t0, trace);

    TTEntry tte = tt.ProbeEntry(hash);
    bool tt_hit = tte.key == hash;
    chess::Move ttMove = tt_hit ? tte.move : chess::Move::NULL_MOVE;

    bool isPV = (beta - alpha > 1);
    int tt_score = tt_hit ? tt.ScoreFromTT(tte.score, ply) : VALUE_NONE;

    if (tte.depth >= depth && tt_hit && NotRootNode) {
        if (tte.flag == static_cast<uint8_t>(FLAGS::EXACTBOUND)) return tt_score;
        if (tte.flag == static_cast<uint8_t>(FLAGS::LOWERBOUND) && tt_score >= beta && !isPV) return tt_score;
        if (tte.flag == static_cast<uint8_t>(FLAGS::UPPERBOUND) && tt_score < alpha && !isPV) return tt_score;
    }

    int static_eval = (!isPV && NotRootNode && !inCheck && beta < VALUE_MATE_IN_PLY && depth <= 7) ? evaluate(board, trace) : -VALUE_NONE;

    if (static_eval != -VALUE_NONE) {
        
        // RFP
        if (depth <= 7 && static_eval - 70 * depth >= beta)
            return static_eval - 70 * depth;

        // RAZOR
        if (depth <= 3 && static_eval + 300 + 60 * depth < alpha)
            return qSearch(board, alpha, beta, ply, ess, ec, t0, trace);
    }

    // Null Move Pruning
    if (
        depth >= 3 && !inCheck && !isPV && shouldNull &&
        beta < VALUE_MATE_IN_PLY && NotRootNode &&

        // A bit messy, but I guess it works.
        (
            board.pieces(chess::PieceType::KNIGHT, side) |
            board.pieces(chess::PieceType::BISHOP, side) |
            board.pieces(chess::PieceType::ROOK, side) |
            board.pieces(chess::PieceType::QUEEN, side)
        ).getBits()
    ) {

        if (static_eval == -VALUE_NONE) static_eval = evaluate(board, trace);

        if (static_eval >= beta) {
            int R = depth >= 7 ? 4 : 3;

            board.makeNullMove();
            int nmp_score = -alphaBeta(board, -beta, -beta + 1, depth - R - 1, ply + 1, ess, ec, t0, trace, false);
            board.unmakeNullMove();

            if (nmp_score >= beta) return nmp_score;
        }
    }

    // IIR
    if (depth >= 4 && !tt_hit && !inCheck) depth--;

    int bestScore = -VALUE_INFINITE;
    chess::Move bestMove = chess::Move::NULL_MOVE;
    int old_alpha = alpha;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    ScoreMoves<false>(board, moves, ess, ply, ttMove);

    for (int i = 0; i < moves.size(); i++) {
        ess.nodes++;

        PickMove(moves, i);
        chess::Move move = moves[i];

        board.makeMove(move);
        
        if (i == 0) {
            score = -alphaBeta(board, -beta, -alpha, depth - 1, ply + 1, ess, ec, t0, trace, true);
        } else {
            score = -alphaBeta(board, -alpha - 1, -alpha, depth - 1, ply + 1, ess, ec, t0, trace, true);
            if (score > alpha && isPV) {
                score = -alphaBeta(board, -beta, -alpha, depth - 1, ply + 1, ess, ec, t0, trace, true);
            }
        }

        board.unmakeMove(move);

        if (score > bestScore) bestScore = score;

        if (score > alpha) {
            alpha = score;
            bestMove = move;
            AddPV(move, ess, ply);
        }

        if (alpha >= beta) {
            if (!board.isCapture(move)) {
                AddKiller(move, ess, ply);

                int bonus = bonus_squared[depth - 1];

                bool side = board.sideToMove() == chess::Color::WHITE;
                int from = move.from().index();
                int to = move.to().index();

                int history_score = (
                    bonus
                    - ess.history[side][from][to]
                    * bonus
                    / 16384 
                );

                ess.history[side][from][to] += history_score;
            }
            break;
        }
    }

    if (moves.empty()) {
        if (inCheck) return mated_in(ply);
        else return 0;
    }

    FLAGS flag = FLAGS::EXACTBOUND;
    if (bestScore >= beta) flag = FLAGS::LOWERBOUND;
    else {
        if (alpha != old_alpha) flag = FLAGS::EXACTBOUND;
        else flag = FLAGS::UPPERBOUND;
    }

    if (!time) tt.StoreIndex(hash, depth, flag, bestScore, bestMove, ply);

    return bestScore;
}



void IterativeDeepening(chess::Board& board, EngineSearchStuff& ess, EngineController& ec) {
    initPV(ess);
    ess.nodes = 0;
    int score = -VALUE_INFINITE;

    chess::Move bestmove = chess::Move::NULL_MOVE;
    clk t0 = std::chrono::steady_clock::now();

    int maxDepth = Load(ec.max_depth);
    if (maxDepth <= 0) maxDepth = MAX_PLY;

    for (int d = 1; d <= maxDepth; d++) {
        ResetHistory(ess);

        Trace trace{}; // Reset on each iter since it's only used for tuning.
        score = alphaBeta(board, -VALUE_INFINITE, VALUE_INFINITE, d, 0, ess, ec, t0, trace, true);

        if (!(Load(ec.is_running)) || checkTime(true, ec, ess, t0)) break;

        bestmove = ess.pvTable[0][0];
        clk now = std::chrono::steady_clock::now();

        uint64_t t = std::chrono::duration_cast<std::chrono::nanoseconds>(now - t0).count();
        stats(d, score, t, ess);
    }

    if (bestmove == chess::Move::NULL_MOVE) bestmove = ess.pvTable[0][0];

    if (bestmove == chess::Move::NULL_MOVE) {
        chess::Movelist moves;
        chess::movegen::legalmoves(moves, board);

        if (moves.size() > 0) {
            bestmove = moves[0];
        } else {
            FlushPrint("bestmove 0000\n");
            return;
        }
    }

    FlushPrint("bestmove " + chess::uci::moveToUci(bestmove) + '\n');
}