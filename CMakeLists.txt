# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Project configuration.
cmake_minimum_required(VERSION 3.0)
project(lingo CXX)
enable_testing()


# Boost dependencies
find_package(Boost 1.55.0 REQUIRED
  COMPONENTS system filesystem)


# Compiler configuration
set(CMAKE_CXX_FLAGS "-Wall -std=c++1y")


# Build configuration
include_directories(. ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})


add_subdirectory(lingo)
add_subdirectory(beaker)
