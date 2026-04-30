#pragma once

#include "../../include/chess.hpp"
#include "../../uci/options/spin.hpp"
#include "../../helpers/search/values.hpp"

#include <vector>

enum class FLAGS {
    NONEBOUND,
    UPPERBOUND,
    LOWERBOUND,
    EXACTBOUND
};

struct TTEntry {
    uint64_t key     = 0;
    int32_t score    = VALUE_NONE;
    uint16_t move = chess::Move::NULL_MOVE;
    uint8_t depth    = 0;
    uint8_t flag     = static_cast<uint8_t>(FLAGS::NONEBOUND);
};

class TranspositionTable {
    size_t size = 1;

    static size_t HashMbOrDefault() {
        constexpr size_t default_hash_mb = 16;
        auto it = spin_entries.find("Hash");
        if (it == spin_entries.end()) return default_hash_mb;

        int configured_hash_mb = it->second.default_value;
        if (configured_hash_mb <= 0) return default_hash_mb;
        return static_cast<size_t>(configured_hash_mb);
    }

    static size_t ComputeTableSize() {
        size_t hash_mb = HashMbOrDefault();
        size_t computed = (hash_mb * 1024ULL * 1024ULL) / sizeof(TTEntry);
        if (computed == 0) return 1;
        return computed;
    }

public:
    std::vector<TTEntry> table;

    TranspositionTable() {
        size = ComputeTableSize();
        table.resize(size);
    }

    void clear() {
        std::fill(table.begin(), table.end(), TTEntry{});
    }

    void resize() {
        size = ComputeTableSize();

        table.clear();
        table.resize(size);

        clear();
    }

    const size_t Index(const uint64_t& key) {
        if (size == 0) return 0;
        return key % size;
    }

    int32_t ScoreToTT(int32_t score, int ply) {
        if (score >= VALUE_TB_WIN_IN_MAX_PLY) return score + ply;
        else if (score <= VALUE_TB_LOSS_IN_MAX_PLY) return score - ply;
        else return score;
    }

    int32_t ScoreFromTT(int32_t score, int ply) {
        if (score >= VALUE_TB_WIN_IN_MAX_PLY) return score - ply;
        else if (score <= VALUE_TB_LOSS_IN_MAX_PLY) return score + ply;
        else return score;
    }

    void StoreIndex(uint64_t key, uint8_t depth, FLAGS flag, int32_t score, chess::Move move, uint8_t ply) {
        size_t index = Index(key);
        auto& entry = table[index];

        if (entry.key != key || entry.move != move.getMoveNum()) entry.move = move.getMoveNum();

        if (entry.key != key || flag == FLAGS::EXACTBOUND || depth + 4 > entry.depth || entry.move != move.getMoveNum()) {
            entry.key   = key;
            entry.depth = depth;
            entry.flag  = static_cast<uint8_t>(flag);
            entry.score = ScoreToTT(score, ply);
            entry.move  = move.getMoveNum();
        }
    }

    TTEntry ProbeEntry(uint64_t key) {
        size_t index = Index(key);
        auto& entry = table[index];

        return entry;
    }

    void Prefetch(uint64_t key) {
        size_t index = Index(key);
        __builtin_prefetch(&table[index]);
    }
    
};

inline TranspositionTable tt;