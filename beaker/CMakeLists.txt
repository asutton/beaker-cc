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


add_executable(beaker 
  main.cpp
  string.cpp
  file.cpp
  cast.cpp
  symbol.cpp
  expr.cpp
  type.cpp
  decl.cpp
  value.cpp
  print.cpp
  equal.cpp
  less.cpp
  hash.cpp
  line.cpp
  location.cpp
  error.cpp
  token.cpp
  lexer.cpp
  parser.cpp
  environment.cpp
  elaborator.cpp
  evaluator.cpp
  generator.cpp)

target_link_libraries(beaker 
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  lingo-llvm
  lingo 
  )
