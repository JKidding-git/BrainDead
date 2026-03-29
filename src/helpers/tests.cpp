#include "tests.hpp"
#include "misc.hpp"

template <TestScoreModes mode>
void TestScore(score input_1, score input_2) {
    if constexpr (mode == ADDITION) {
        int32_t expected_middlegame = input_1.middlegame + input_2.middlegame;
        int32_t expected_endgame = input_1.endgame + input_2.endgame;

        score got = input_1 + input_2;

        FlushPrint("info string test middlegame (+) expected: " + std::to_string(expected_middlegame) + " got " + std::to_string(got.middlegame) + '\n');
        FlushPrint("info string test endgame (+) expected: " + std::to_string(expected_endgame) + " got " + std::to_string(got.endgame) + '\n');
    } else if constexpr (mode == SUBTRACTION) {
        int32_t expected_middlegame = input_1.middlegame - input_2.middlegame;
        int32_t expected_endgame = input_1.endgame - input_2.endgame;

        score got = input_1 - input_2;

        FlushPrint("info string test middlegame (-) expected: " + std::to_string(expected_middlegame) + " got " + std::to_string(got.middlegame) + '\n');
        FlushPrint("info string test endgame (-) expected: " + std::to_string(expected_endgame) + " got " + std::to_string(got.endgame) + '\n');
    } else if constexpr (mode == MULTIPLICATION) {
        int32_t expected_middlegame = input_1.middlegame * input_2.middlegame;
        int32_t expected_endgame = input_1.endgame * input_2.endgame;

        score got = input_1 * input_2;

        FlushPrint("info string test middlegame (*) expected: " + std::to_string(expected_middlegame) + " got " + std::to_string(got.middlegame) + '\n');
        FlushPrint("info string test endgame (*) expected: " + std::to_string(expected_endgame) + " got " + std::to_string(got.endgame) + '\n');
    } else {
        FlushPrint("info string ERROR: Please select a valid mode!\n");
    }
}

void RunTests() {
    words_vector test_words = SplitWords("hello world uci");
    if (test_words.size() == 3 && test_words[0] == "hello" && test_words[1] == "world" && test_words[2] == "uci") {
        FlushPrint("SplitWords test passed.\n");
    } else {
        FlushPrint("SplitWords test failed.\n");
    }
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            score s1 = S(i * 10, i * 5);
            score s2 = S(j * 15, j * 2);
            
            TestScore<ADDITION>(s1, s2);
            TestScore<SUBTRACTION>(s1, s2);
            TestScore<MULTIPLICATION>(s1, s2);
        }
    }
}