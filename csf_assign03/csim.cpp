#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cstdlib>
#include "cache_structure.h"

bool check_power_two(int val) {
  if (val <= 0) {return false;}
  if (val == 1) {return true;}
  if ((val & (val-1)) == 0) {return true;}
  
  return false;
}

bool set_write_allocate(Parameters* params, std::string& arg) {
  if (arg == "write-allocate") {
    params->write_allocate = 1;
  } else if (arg == "no-write-allocate") {
    params->write_allocate = 0;
  } else {
    std::cerr << "Invalid write-allocate argument" << std::endl;
    return false;
  }
  return true;
}

bool set_write_policy(Parameters* params, std::string& arg) {
  if (arg == "write-through") {
    params->write_through = 1;
  } else if (arg == "write-back") {
    params->write_through = 0;
  } else {
    std::cerr << "Invalid write-policy argument" << std::endl;
    return false;
  }
  return true;
}

bool set_eviction_policy(Parameters* params, std::string& arg) {
  if (arg == "fifo") {
    params->eviction_policy = 0;
  } else if (arg == "lru") {
    params->eviction_policy = 1;
  } else {
    std::cerr << "Invalid eviction-policy argument" << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char* argv[]) {
  //TODO: Implement
  //Check for correct number of arguments
  if (argc != 7) {
    std::cerr << "Incorrect number of arguments" << std::endl;
    return 1;
  }
  
  int num_sets = strtol(argv[1], nullptr, 10);
  int num_blocks = strtol(argv[2], nullptr, 10);
  int block_size = strtol(argv[3], nullptr, 10);
  std::string write_allocate = argv[4];
  std::string write_policy = argv[5];
  std::string eviction_policy = argv[6];
  
  //Check args 1,2,3 are powers of 2
  if (!check_power_two(num_sets) || !check_power_two(num_blocks) || !check_power_two(block_size)) {
    std::cerr << "Arguments 1, 2, and 3 must be powers of 2" << std::endl;
    return 2;
  }
  
  if (block_size < 4) {
    std::cerr << "Block size must be at least 4 bytes" << std::endl;
    return 3;
  }
  
  //Initialize parameters struct
  Parameters params;
  
  //Assign each argument to the correct parameter
  params.num_sets = num_sets;
  params.num_slots = num_blocks;
  params.slot_size = block_size;
  
  if (!set_write_allocate(&params, write_allocate)) {
    return 4;
  }
  
  if (!set_write_policy(&params, write_policy)) {
    return 5;
  }
  
  if (!set_eviction_policy(&params, eviction_policy)) {
    return 6;
  }
  
  //Initialize Cache object
  Cache cache = Cache(&params);
  
  //Simulation Stats
  unsigned totalLoads = 0;
  unsigned totalStores = 0;
  unsigned loadHits = 0;
  unsigned loadMisses = 0;
  unsigned storeHits = 0;
  unsigned storeMisses = 0;
  unsigned totalCycles = 0;
  
  //simulate cache functions
  std::string instruction, hex_address, ignore;
  unsigned address;
  
  unsigned nOffsetBits = cache.log2(cache.params->slot_size);
  unsigned nIndexBits = cache.log2(cache.params->num_sets);
  
  while (std::cin >> instruction >> hex_address >> ignore) {
    //convert hex_address to unsigned int
    address = std::stoul(hex_address, nullptr, 16);
    
    //get index
    unsigned index = (address >> nOffsetBits) & ((1 << nIndexBits) - 1);
    
    //get tag
    unsigned tag = address >> (nOffsetBits + nIndexBits);
    
    Set set = cache.sets[index];
    std::map<unsigned, Slot*>::iterator it = set.slot_map.find(tag);
    bool slot_exists = (it != set.slot_map.end());
    Slot *slot;
    if (slot_exists) {
      slot = it->second;
    }
    
//    if (slot_exists) {
//      slot = cache.get_slot(tag, &set);
//    }
    
    //check if load or store
    if (instruction == "l") {
      totalLoads++;
      if (!slot_exists) {
        loadMisses++;
        cache.miss(index, tag, true, totalCycles);
      } else {
        loadHits++;
        totalCycles++;
        slot->access_ts = totalCycles;
      }
    } else if (instruction == "s") {
      totalStores++;
      if (cache.params->write_through == 1) { totalCycles += 100; }
      if (!slot_exists) {
        storeMisses++;
        if (cache.params->write_allocate == 1) {
          cache.miss(index, tag, false, totalCycles);
        }
      } else {
        storeHits++;
        if (cache.params->write_through == 0) {
          totalCycles++;
          slot->dirty = true;
        }
        slot->access_ts = totalCycles;
      }
    } else {
      std::cerr << "Invalid instruction" << std::endl;
      return 7;
    }
  }
  
//  std::cout << "Total loads: " << totalLoads << std::endl;
//  std::cout << "Total stores: " << totalStores << std::endl;
//  std::cout << "Load hits: " << loadHits << std::endl;
//  std::cout << "Load misses: " << loadMisses << std::endl;
//  std::cout << "Store hits: " << storeHits << std::endl;
//  std::cout << "Store misses: " << storeMisses << std::endl;
//  std::cout << "Total cycles: " << totalCycles << std::endl;
  std::cout << "Total hits: " << loadHits + storeHits << std::endl;
  std::cout << "Total misses: " << loadMisses + storeMisses << std::endl;
  std::cout << "Total cycles: " << totalCycles << std::endl;
  
  return 0;
}