#pragma once

#include <unordered_map>
#include <chrono>

#include "../../helpers/misc.hpp"
#include "spin.hpp"

using umap_check = std::unordered_map<std::string, bool>;
inline umap_check check_entries;


void AddCheck(const std::string& name, bool value, umap_check& entries);
void ChangeCheck(const std::string& name, bool new_value, umap_check& entries);
void ShowAllChecksForUCI(umap_check& entries);
void SetOptionCheck(words_vector& word, umap_check& entries);

void PerfTestAddCheck();