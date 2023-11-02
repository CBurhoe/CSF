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
    for (unsigned j = 0; j < params->num_slots; j++) {
      Slot slot;
      set.slots.push_back(slot);
    }
    this->sets.push_back(set);
  }
//  this->sets = std::vector<Set>(params->num_sets);
//  for (unsigned i = 0; i < params->num_sets; i++) {
//    //Fixme: allocate sets of slots without looping through every slot in each set
//
//  }
  unsigned long slot_size = this->params->slot_size >> 2;
  unsigned long memoryCycles = (slot_size << 6) + (slot_size << 5) + (slot_size << 2);
  read_write_length = memoryCycles;
}

unsigned Cache::log2(unsigned n) {
  unsigned val = 0;
  while (n != 1) {
    n >>= 1;
    val++;
  }
  return val;
}

Set Cache::get_set(unsigned index) {
  return this->sets.at(index);
}

Slot Cache::get_slot(unsigned tag, Set* set) {
  std::map<unsigned, Slot*>::iterator it = set->slot_map.find(tag);
  Slot* slotPtr;
  slotPtr = it->second;
  
  return *slotPtr;
}

void Cache::miss(unsigned index, unsigned tag, bool load, unsigned &totalCycles) {
  totalCycles += this->read_write_length;
  
  Set set = this->sets.at(index);
  std::vector<Slot> slots = set.slots;
  
  unsigned long new_index = 0;
  // if there is no empty block, evict
  if (slots.size() == set.slot_map.size()) {
    new_index = evict(index, totalCycles);
//    replace(new_index, index, tag, load, totalCycles);
  } else {
    for (unsigned i = 0; i < slots.size(); i++) {
      if (!slots.at(i).full) {
        new_index = i;
        break;
      }
    }
  }
  
  
//  // if there is no empty block, evict
//  if (new_index == -1) {
//    new_index = evict(index, totalCycles);
//  }
  replace(new_index, index, tag, load, totalCycles);
}

unsigned long Cache::evict(unsigned int index, unsigned int &totalCycles) {
  std::vector<Slot> *slots = &this->sets.at(index).slots;
  unsigned j = 0;
  if (this->params->eviction_policy == 1) {
    for (unsigned i = 1; i < this->params->num_slots; i++) {
      if (slots->at(i).access_ts < slots->at(j).access_ts) {
        j = i;
      }
    }
  } else {
    for (unsigned i = 1; i < this->params->num_slots; i++) {
      if (slots->at(i).load_order < slots->at(j).load_order) {
        j = i;
      }
    }
  }
  if (slots->at(j).dirty) {
    totalCycles += this->read_write_length;
  }
  this->sets.at(index).slot_map.erase(slots->at(j).tag);
  return j;
}

void Cache::replace(unsigned int new_index, unsigned int index, unsigned int tag, bool load,
                    unsigned int &totalCycles) {
  this->sets.at(index).slots.at(new_index).tag = tag;
  this->sets.at(index).slots.at(new_index).full = true;
  this->sets.at(index).slots.at(new_index).access_ts = totalCycles;
  this->sets.at(index).slots.at(new_index).load_order = totalCycles;
  
  if (!load && this->params->write_through == 0) {
    this->sets.at(index).slots.at(new_index).dirty = true;
  }
  
  this->sets.at(index).slot_map.insert(std::pair<unsigned, Slot*>(tag, &this->sets.at(index).slots.at(new_index)));
}