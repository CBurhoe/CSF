//
// Created by casey on 10/11/2023.
//

#ifndef CACHE_STRUCTURE_H
#define CACHE_STRUCTURE_H

#include "cache_structure.h"
#include <iostream>
#include <stdint.h>
#include <vector>

struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts,
        access_ts;
};

struct Set {
    std::vector<Slot> slots;
};

struct Cache {
    std::vector<Set> sets;
};


#endif // CACHE_STRUCTURE_H
