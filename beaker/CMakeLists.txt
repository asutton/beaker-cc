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
# execute_process(COMMAND llvm-config --libfiles OUTPUT_VARIABLE LLVM_LIBS_IN)
execute_process(COMMAND llvm-config --libs OUTPUT_VARIABLE LLVM_LIBS_STR)

string(STRIP ${LLVM_INCLUDE_STR} LLVM_INCLUDE_DIR)
string(STRIP ${LLVM_LIBRARY_STR} LLVM_LIBRARY_DIR)
string(STRIP ${LLVM_LIBS_STR} LLVM_LIBS_IN)

set(LLVM_LIBS)
separate_arguments(LLVM_LIBS_ARGS UNIX_COMMAND ${LLVM_LIBS_IN})
foreach(lib ${LLVM_LIBS_ARGS})
  string(SUBSTRING ${lib} 2 -1 s1)
  list(APPEND LLVM_LIBS ${s1})
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
  line.cpp
  location.cpp
  cast.cpp
  symbol.cpp
  expr.cpp
  type.cpp
  decl.cpp
  value.cpp
  print.cpp
  equal.cpp
  less.cpp
  convert.cpp
  error.cpp
  token.cpp
  lexer.cpp
  parser.cpp
  environment.cpp
  elaborator.cpp
  evaluator.cpp
  generator.cpp)


# Note that the order of libraries is established experimentally.
# It really shouldn't matter, but apparently it does. Yeah.
target_link_libraries(beaker 
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${LLVM_LIBS}
  dl        # FIXME: This is dumb
  tinfo     # FIXME: Also dumb
  ${CMAKE_THREAD_LIBS_INIT}
  lingo)
