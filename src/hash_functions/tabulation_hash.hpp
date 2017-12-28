#ifndef TABULATION_HASH_H
#define TABULATION_HASH_H

//#define TWISTED // optimization for twisted tabulation

#include "../utils/memory.hpp"

#include <random>
#include <limits>
#include <vector>
#include <iostream>
#include <array>
#include <algorithm>
#include <memory>

// Zobrist hashing a.k.a Simple Tabulation Hashing
// Using Mersenne Twister 64 bit pseudorandom number generator, seeded by
// Minimal Standard generator (simple multiplicative congruential)
template<class Entry>
class TabulationHash {
    // for seeding Mersenne Twister vector of internal states
    static size_t master_seed;
    static std::minstd_rand master_eng;
    static std::uniform_int_distribution<unsigned> master_dis;

    // actual number generator for zobrist hashing
    static unsigned get_rand_seed();
    static std::unique_ptr<std::mt19937_64> mt_ptr; 
    static std::uniform_int_distribution<std::size_t> dis;

    size_t get_rand_bitstring() const;

    std::vector< std::vector<size_t> > table;
        
 public:
    TabulationHash();
        
    std::size_t operator()(const Entry& entry) const; // hash value
};

template<class Entry>
std::size_t TabulationHash<Entry>::master_seed = 1; // default seed

template<class Entry>
std::minstd_rand TabulationHash<Entry>::master_eng(master_seed);

template<class Entry>
std::uniform_int_distribution<unsigned>
TabulationHash<Entry>::master_dis(0, std::numeric_limits<unsigned>::max());

template<class Entry>
unsigned TabulationHash<Entry>::get_rand_seed() {
    return master_dis(master_eng);
}

template<class Entry>
std::uniform_int_distribution<std::size_t>
TabulationHash<Entry>::dis(0, std::numeric_limits<std::size_t>::max());

template<class Entry>
std::unique_ptr<std::mt19937_64> TabulationHash<Entry>::mt_ptr = nullptr;
    
template<class Entry>
TabulationHash<Entry>::TabulationHash() 
{
    // initialize seeds for mersenne twister
    if (!mt_ptr) {
        
        std::array<unsigned, std::mt19937_64::state_size> seed_data;
        
    
    std::generate(std::begin(seed_data), std::end(seed_data),
                                    get_rand_seed);
        
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        
        mt_ptr = memory::make_unique<std::mt19937_64>(std::mt19937_64(seq));
       
    }
        
    // resize table to match dimensions of variables and their  values
    table.resize(Entry::get_n_var());
    for (std::size_t i = 0; i < table.size(); ++i) {
        table[i].resize(Entry::get_n_val());
    }

    // fill table with random bitstrings (64 bit)
    for (auto& var : table) {
        for (auto& val : var) {
            val = get_rand_bitstring();
        }
    }
}

template<class Entry>
std::size_t TabulationHash<Entry>::get_rand_bitstring() const {
    return dis(*mt_ptr);
}

template<class Entry>
std::size_t TabulationHash<Entry>::operator()(const Entry& entry) const {
    std::size_t hash_value = 0;
#ifdef TWISTED
    std::size_t i = 0;
    for (; i < table.size() - 1; ++i) {
        hash_value ^= table[i][entry[i]];
    }
        
    hash_value ^= table[i][(entry[i] ^ hash_value) % table[i].size()];
#else
    for (std::size_t i = 0; i < table.size(); ++i) {
        hash_value ^= table[i][entry[i]];
    }
#endif
    return hash_value;
}

#endif
