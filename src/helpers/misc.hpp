#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

using words_vector = std::vector<std::string>;

/// @brief Prints a string and then flushes it.
/// @param input String input.
inline void FlushPrint(std::string input) {
    std::cout << input << std::flush;
}

/// @brief Splits the string input into a vector of words.
/// @param input String input.
/// @return A vector of words.
inline words_vector SplitWords(const std::string& input) {

    // if the input is empty, return a vector with ""
    if (input.empty()) return {""};

    words_vector output;
    std::stringstream ss(input);

    std::string word;
    while (ss >> word) {
        output.push_back(word);
    }

    return output;
}