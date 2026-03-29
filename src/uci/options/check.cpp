#include "check.hpp"

void AddCheck(const std::string& name, bool value, umap_check& entries) {

    // No spaces just to make my life a little easier.
    assert(name.find(' ') == std::string::npos);

    // Name should be unique and not available in other UCI options entries.
    assert(!entries.contains(name) && !spin_entries.contains(name));

    // Store the result to the entry.
    entries.emplace(name, value);
}

void ChangeCheck(const std::string& name, bool new_value, umap_check& entries) {

    // No spaces just to make my life a little easier.
    assert(name.find(' ') == std::string::npos);

    // Name must be the same in order for the spin to change.
    assert(entries.contains(name));

    // Overwrite the entry's default value.
    entries[name] = new_value;
}

void ShowAllChecksForUCI(umap_check& entries) {

    // Loops through the entry.
    for (const auto& entry : entries) {

        // Convert the boolean value to string.
        std::string bool_to_str = entry.second ? "true" : "false";

        // Outputs the result info to the terminal.
        FlushPrint(
            "option name " + entry.first +
            " type check default " + bool_to_str +
            '\n'
        );
    }
}

void SetOptionCheck(words_vector& word, umap_check& entries) {

    // Get the Check name.
    const std::string& name = word[2];

    // Skip if the entry's name isn't the same.
    if (!entries.contains(name)) return;

    bool value = entries[name];

    // Check if the fifth word is true, else. False.
    if (word[4] == "true")       value = true;
    else if (word[4] == "false") value = false;

    // Finds the name in the entry, if found, change the Check value.
    auto it = entries.find(name);
    if (it != entries.end()) {
        ChangeCheck(name, value, entries);
    }
}

// Just a little performance test.
void PerfTestAddCheck() {
    umap_check perf_test_entries;

    // Start the timer.
    auto start_timer = std::chrono::high_resolution_clock::now();

    // Loop until "i" is n - 1.
    for (int i = 0; i < 100000; i++) {
        AddCheck("CheckTest" + std::to_string(i), true, perf_test_entries);
    }

    // Stop the timer
    auto stop_timer = std::chrono::high_resolution_clock::now();

    // Gets the time in milliseconds.
    std::chrono::duration<double, std::milli> duration = stop_timer - start_timer;
    double time_in_ms = duration.count();

    // Outputs the result to the terminal.
    FlushPrint("PerfTestAddCheck (100k iterations) took: " + std::to_string(time_in_ms) + " ms\n");
}