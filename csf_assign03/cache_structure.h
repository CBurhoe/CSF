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

struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts,
        access_ts;
};

struct Set {
    std::vector<Slot> slots;
    std::map<uint32_t, Slot*> slot_map;
};

class Cache {
private:
    std::vector<Set> sets;
    
public:
    Cache(uint32_t num_sets, uint32_t num_blocks, uint32_t block_size);
    unsigned load();
    unsigned store();
};



#endif // CACHE_STRUCTURE_H
