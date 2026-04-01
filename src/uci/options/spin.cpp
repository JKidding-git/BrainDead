#include "spin.hpp"
#include <cassert>

void AddSpin(const std::string& name, int default_value, int minimum_value, int maximum_value, umap_spin& entries) {
    
    // No spaces just to make my life a little easier.
    assert(name.find(' ') == std::string::npos);
    
    // Basic error checks.
    assert(default_value >= minimum_value);
    assert(default_value <= maximum_value);
    assert(minimum_value <= maximum_value);

    // Name should be unique and not available in other UCI options entries.
    assert(!entries.contains(name) && !check_entries.contains(name));

    // Store the result temporarily.
    SpinStruct result;
    result.default_value = default_value;
    result.minimum_value = minimum_value;
    result.maximum_value = maximum_value;

    // Store the result to the entry.
    entries.emplace(name, result);
}

void ChangeSpin(const std::string& name, int new_value, umap_spin& entries) {

    // No spaces just to make my life a little easier.
    assert(name.find(' ') == std::string::npos);

    // Basic error checks.
    assert(new_value >= entries[name].minimum_value);
    assert(new_value <= entries[name].maximum_value);

    // Name must be the same in order for the spin to change.
    assert(entries.contains(name));

    // Overwrite the entry's default value.
    entries[name].default_value = new_value;
}

void ShowAllSpinsForUCI(umap_spin& entries) {

    // Loops through the entry.
    for (const auto& entry : entries) {

        // Outputs the result info to the terminal.
        FlushPrint(
            "option name " + entry.first +
            " type spin default " + std::to_string(entry.second.default_value) +
            " min " + std::to_string(entry.second.minimum_value) +
            " max " + std::to_string(entry.second.maximum_value) +
            '\n'
        );
    }
}

void SetOptionSpin(words_vector& word, umap_spin& entries) {

    // Get the Spin name.
    const std::string& name = word[2];

    // Skip if the entry's name isn't the same.
    if (!entries.contains(name)) return;

    int value;

    // Get the Spin value.
    // stoi means "string to integer".
    try {value = std::stoi(word[4]);}
    catch (...) {return;}

    // Finds the name in the entry, if found, change the Spin value.
    auto it = entries.find(name);
    if (it != entries.end()) {
        ChangeSpin(name, value, entries);
    }
}


// Just a little performance test.
void PerfTestAddSpin() {
    umap_spin perf_test_entries;

    // Start the timer.
    auto start_timer = std::chrono::high_resolution_clock::now();

    // Loop until "i" is n - 1.
    for (int i = 0; i < 100000; i++) {
        AddSpin("SpinTest" + std::to_string(i), 50, 0, 100, perf_test_entries);
    }

    // Stop the timer
    auto stop_timer = std::chrono::high_resolution_clock::now();

    // Gets the time in milliseconds.
    std::chrono::duration<double, std::milli> duration = stop_timer - start_timer;
    double time_in_ms = duration.count();

    // Outputs the result to the terminal.
    FlushPrint("PerfTestAddSpin (100k iterations) took: " + std::to_string(time_in_ms) + " ms\n");
}