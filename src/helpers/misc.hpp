#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>

using words_vector = std::vector<std::string>;

/// @brief Prints a string and then flushes it.
/// @param input String input.
inline void FlushPrint(std::string input) {
    std::cout << input << std::flush;
}

/// @brief Splits the string input into a vector of words.
/// @param input String input.
/// @return A vector of words.
inline words_vector SplitWords(std::string input) {

    // Initialize output vector.
    words_vector output;

    // Uses boost to split the words up.
    // A lil overkill, but it works I guess.
    boost::split(output, input, boost::is_any_of(" "), boost::token_compress_on);

    // Outputs the output (lmao)
    return output;
}