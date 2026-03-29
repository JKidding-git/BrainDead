#pragma once

#include <unordered_map>
#include <chrono>

#include "../../helpers/misc.hpp"
#include "check.hpp"

struct SpinStruct {
    int default_value;
    int minimum_value;
    int maximum_value;
};

// Just to make it easier to look at.
using umap_spin = std::unordered_map<std::string, SpinStruct>;

inline umap_spin spin_entries;

void AddSpin(const std::string& name, int default_value, int minimum_value, int maximum_value, umap_spin& entries);
void ChangeSpin(const std::string& name, int new_value, umap_spin& entries);
void ShowAllSpinsForUCI(umap_spin& entries);
void SetOptionSpin(words_vector& word, umap_spin& entries);
void PerfTestAddSpin();