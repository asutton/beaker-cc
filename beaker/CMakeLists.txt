# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Project configuration.
cmake_minimum_required(VERSION 3.0)
project(lingo CXX)
enable_testing()


# Boost dependencies
find_package(Boost 1.55.0 REQUIRED
  COMPONENTS system filesystem)


# Determine LLVM configuration from llvm-config
#
# Note that these all come in as strings that need to
# be "sanitized" prior to processing. For the LLVM
# libraries, we need to make separate targets. 
execute_process(COMMAND llvm-config --includedir OUTPUT_VARIABLE LLVM_INCLUDE_STR)
execute_process(COMMAND llvm-config --libdir OUTPUT_VARIABLE LLVM_LIBRARY_STR)
execute_process(COMMAND llvm-config --libfiles OUTPUT_VARIABLE LLVM_LIBS_IN)
string(STRIP ${LLVM_INCLUDE_STR} LLVM_INCLUDE_DIR)
string(STRIP ${LLVM_LIBRARY_STR} LLVM_LIBRARY_DIR)
set(LLVM_LIBS)
separate_arguments(LLVM_LIBS_ARGS UNIX_COMMAND ${LLVM_LIBS_IN})
foreach(lib ${LLVM_LIBS_ARGS})
  list(APPEND LLVM_LIBS ${lib})
endforeach()


# Compiler configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y")


# Build configuration
include_directories(. ${Boost_INCLUDE_DIRS} ${LLVM_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS} ${LLVM_LIBRARY_DIR})


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
  ${LLVM_LIBS}
  lingo-llvm
  lingo)
