#pragma once
#include "evaluation/score.hpp"

enum TestScoreModes {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    NONE,
};

void TestScore(score input_1, score input_2);
void RunTests();