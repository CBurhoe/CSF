#include "cache_structure.h"
#include <iostream>
#include <cstdint>
#include <vector>

//
// Created by casey on 10/11/2023.
//

Cache::Cache(Parameters* params): params(params) {
  //todo
  for (unsigned i = 0; i < params->num_sets; i++) {
    Set set;
    for (unsigned j = 0; j < params->num_blocks; j++) {
      Slot slot;
      set.slots.push_back(slot);
    }
    this->sets.push_back(set);
  }
}

unsigned int Cache::load() {
  //todo
  return 0;
}

unsigned int Cache::store() {
  //todo
  return 0;
}

void Cache::run_simulation() {
  //todo
}