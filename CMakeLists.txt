# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Project configuration.
cmake_minimum_required(VERSION 3.1)
project(lingo C CXX)
enable_testing()


# Boost dependencies
find_package(Boost 1.55.0 REQUIRED
  COMPONENTS system filesystem)


# LLVM Configuration
find_package(Threads REQUIRED)
find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIBRARIES
  core support)

# Compiler configuration
set(CMAKE_CXX_FLAGS "-Wall -std=c++1y")


# Configuration for lingo dependencies
set(LINGO_INCLUDE_DIR ./lingo)


# Build configuration
include_directories(
  ${Boost_INCLUDE_DIRS}
  ${LLVM_INCLUDE_DIRS}
  ${LINGO_INCLUDE_DIR})

link_directories(
  ${Boost_LIBRARY_DIRS}
  ${LLVM_LIBRARY_DIRS})


add_subdirectory(lingo)
add_subdirectory(beaker)
