# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Project configuration.
cmake_minimum_required(VERSION 3.1)
project(lingo CXX)
enable_testing()


# Boost dependencies
find_package(Boost 1.55.0 REQUIRED
  COMPONENTS system filesystem)


# Compiler configuration
set(CMAKE_CXX_FLAGS "-Wall -std=c++1y")


# Configuration for lingo dependencies
set(LINGO_INCLUDE_DIR ./lingo)

# Configuration for llvm dependencies
set(LLVM_INCLUDE_DIR ./llvm)


# Build configuration
include_directories(
  ${Boost_INCLUDE_DIRS} 
  ${LINGO_INCLUDE_DIR} 
  ${LLVM_INCLUDE_DIR})

link_directories(
  ${Boost_LIBRARY_DIRS})


add_subdirectory(lingo)
add_subdirectory(llvm)
add_subdirectory(beaker)
