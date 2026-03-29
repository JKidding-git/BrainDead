#include "position.hpp"

void UCIPosition(words_vector& word, UCIController& uci_controller, chess::Board& board) {
    uint8_t start_of_moves_index = 0;

    if (word[1] == "fen") {
        start_of_moves_index = 8;

        // Combine the words into a full FEN.
        std::string completed_fen = word[2] + " " + word[3] + " " + word[4] + " " + word[5] + " " + word[6] + " " + word[7];
        if (uci_controller.debug) FlushPrint("info string fen got: \"" + completed_fen + "\"\n");

        board.setFen(completed_fen);
    } else if (word[1] == "startpos") {
        start_of_moves_index = 2;

        board.setFen(chess::constants::STARTPOS);
    }

    if (word[start_of_moves_index] == "moves") {

        // Loop through moves
        for (int moves_index = start_of_moves_index + 1; moves_index < word.size(); moves_index++) {
            if (uci_controller.debug) FlushPrint("info string move got: \"" + word[moves_index] + "\"\n");

            chess::Move move = chess::uci::uciToMove(board, word[moves_index]);
            board.makeMove(move);
        }
    }
}