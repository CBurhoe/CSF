//
// Created by casey on 10/11/2023.
//

#ifndef CACHE_STRUCTURE_H
#define CACHE_STRUCTURE_H

#include "cache_structure.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <map>

struct Parameters {
    unsigned num_sets; // positive power of 2
    unsigned num_blocks; // positive power of 2
    unsigned block_size; // positive power of 2, at least 4 bytes
    unsigned write_allocate; // 0 for no-write-allocate, 1 for write-allocate
    unsigned write_through; // 0 for write-back, 1 for write-through
    unsigned eviction_policy; // 0 for fifo, 1 for lru
};

struct Slot {
    unsigned tag;
    bool full = false; // if false, slot is empty
    bool dirty = false; // if true, data must be written to memory before evicting
    unsigned load_order = 0;
    unsigned access_ts = 0;
};

struct Set {
    std::vector<Slot> slots;
    std::map<uint32_t, Slot*> slot_map;
};

class Cache {
private:
    std::vector<Set> sets;
    Parameters* params;
public:
    Cache(Parameters* params);
    unsigned load();
    unsigned store();
};



#endif // CACHE_STRUCTURE_H
