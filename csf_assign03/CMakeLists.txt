cmake_minimum_required(VERSION 3.26)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_library(untitled csim.cpp
        cache_structure.cpp
        cache_structure.h)
