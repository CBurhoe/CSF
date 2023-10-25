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
  
  //Initialize Cache object
  
  //simulate cache functions
  
  return 0;
}