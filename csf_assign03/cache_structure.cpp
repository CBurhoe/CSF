#include "cache_structure.h"
#include <iostream>
#include <stdint.h>
#include <vector>

//
// Created by casey on 10/11/2023.
//

struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts,
            access_ts;
};

