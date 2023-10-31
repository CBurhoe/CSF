//
// Created by casey on 10/11/2023.
//

#ifndef CACHE_STRUCTURE_H
#define CACHE_STRUCTURE_H

#include <iostream>
#include <cstdint>
#include <vector>
#include <map>

struct Parameters {
    unsigned num_sets = 0; // positive power of 2
    unsigned num_slots = 0; // positive power of 2
    unsigned slot_size = 0; // positive power of 2, at least 4 bytes
    unsigned write_allocate = 0; // 0 for no-write-allocate, 1 for write-allocate
    unsigned write_through = 0; // 0 for write-back, 1 for write-through
    unsigned eviction_policy = 0; // 0 for fifo, 1 for lru
};

struct Slot {
    unsigned tag = 0;
    bool full = false; // if false, slot is empty
    bool dirty = false; // if true, data must be written to memory before evicting
    unsigned load_order = 0; // used for FIFO
    unsigned access_ts = 0; // used for LRU
};

struct Set {
    std::vector<Slot> slots;
    std::map<uint32_t, Slot*> slot_map;
};

class Cache {
private:
    std::vector<Set> sets;
    unsigned read_write_length;
public:
    Parameters* params;
    explicit Cache(Parameters* params);
    unsigned log2(unsigned n);
    Set get_set(unsigned index);
    static Slot get_slot(unsigned tag, Set* set);
    void miss(unsigned index, unsigned tag, bool load, unsigned &totalCycles);
    unsigned long evict(unsigned index, unsigned &totalCycles);
    void replace(unsigned new_index, unsigned index, unsigned tag, bool load, unsigned &totalCycles);
};



#endif // CACHE_STRUCTURE_H
