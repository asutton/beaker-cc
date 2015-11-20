# Copyright (c) 2015 Andrew Sutton
# All rights reserved

# Project configuration
cmake_minimum_required(VERSION 3.1)
project(beaker C CXX)
enable_testing()

# Boost dependencies
find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem)

# LLVM dependencies
find_package(LLVM 3.6 REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIBRARIES core support)

# Compiler configuration
set(CMAKE_CXX_FLAGS "-Wall -std=c++1y")

add_subdirectory(lingo)
add_subdirectory(beaker)
