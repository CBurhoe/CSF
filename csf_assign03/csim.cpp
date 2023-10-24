#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cstdlib>
#include "cache_structure.h"

int main(int argc, char **argv) {
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
  
  
  return 0;
}