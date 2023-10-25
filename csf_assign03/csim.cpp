#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cstdlib>
#include "cache_structure.h"

bool check_power_two(int val) {
  if (val <= 0) {return false;}
  
  return (val & (val-1)) == 0;
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
  params.num_blocks = num_blocks;
  params.block_size = block_size;
  
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
  
  unsigned nOffsetBits = cache.log2(cache.params->block_size);
  unsigned nIndexBits = cache.log2(cache.params->num_sets);
  
  
  return 0;
}