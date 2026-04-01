#include "search.hpp"
#include "../../helpers/misc.hpp"
#include "../evaluation/evaluate.hpp"
#include "move_picker.hpp"

bool isQuiet(bool isCapture, bool isPromotion, bool isInCheck) {
    return !isCapture && !isPromotion && !isInCheck;
}

int qSearch(chess::Board& board, int alpha, int beta, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0) {
    if (!(Load(ec.is_running)) || checkTime(false, ec, ess, t0)) return 0;
    if (ply >= MAX_PLY) return evaluate(board);

    int32_t bestValue = evaluate(board);
    if (bestValue >= beta) return bestValue;
    if (bestValue > alpha) alpha = bestValue;

    chess::Movelist moves;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(moves, board);
    ScoreMoves<true>(board, moves);


    for (int i = 0; i < moves.size(); ++i) {
        ++ess.nodes;

        PickMove(moves, i);
        chess::Move move = moves[i];

        board.makeMove(move);
        int score = -qSearch(board, -beta, -alpha, ply + 1, ess, ec, t0);
        board.unmakeMove(move);

        if (score > bestValue) {
            bestValue = score;
            if (score > alpha) {
                alpha = score;
                if (score >= beta) break;
            }
        }
    }

    return bestValue;
}


int alphaBeta(chess::Board& board, int alpha, int beta, int depth, int ply, EngineSearchStuff& ess, EngineController& ec, clk t0) {
    bool time = checkTime(false, ec, ess, t0);
    if (time) return 0;
    if (ply >= MAX_PLY) return evaluate(board);

    
    ess.pvLength[ply] = ply;
    bool RootNode = ply == 0;

    if (!RootNode) {
        if (board.isRepetition(1)) return -5;
        if (board.isHalfMoveDraw()) return 0;

        // Mate Distance Pruning
        alpha = std::max(alpha, mated_in(ply));
        beta = std::min(beta, mate_in(ply + 1));
        if (alpha >= beta) return alpha;
    }

    if (depth <= 0) return qSearch(board, alpha, beta, ply, ess, ec, t0);

    int bestScore = -VALUE_INFINITE;
    chess::Move bestMove = chess::Move::NULL_MOVE;
    bool inCheck = board.inCheck();

    int madeMoves = 0;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    ScoreMoves<false>(board, moves);

    for (int i = 0; i < moves.size(); ++i) {
        ess.nodes++;

        PickMove(moves, i);
        chess::Move move = moves[i];

        board.makeMove(move);
        int score = -alphaBeta(board, -beta, -alpha, depth - 1, ply + 1, ess, ec, t0);
        board.unmakeMove(move);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
            AddPV(move, ess, ply);

            if (score > alpha) {
                alpha = score;
                if (score >= beta) {
                    break;
                }
            }
        }
    }

    if (moves.empty()) {
        if (inCheck) return mated_in(ply);
        else return 0;
    }
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

    for (int d = 1; d <= maxDepth; ++d) {
        score = alphaBeta(board, -VALUE_INFINITE, VALUE_INFINITE, d, 0, ess, ec, t0);

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