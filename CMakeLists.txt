# Copyright (c) 2015 Andrew Sutton
# All rights reserved

# Project configuration
cmake_minimum_required(VERSION 3.1)
project(beaker C CXX)
enable_testing()

# Boost dependencies
find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem program_options)

# LLVM dependencies
find_package(LLVM 3.6 REQUIRED CONFIG)
llvm_map_components_to_libnames(LLVM_LIBRARIES core)

# FIXME: The discovery of additional tools should probably
# be a runtime configuration issue. That is, we should use
# the environment or a configuration library to register the
# availability of tools and then select among those based
# on the requested compilation task.
#
# For now this is probably fine.
find_program(LLVM_IR_COMPILER llc PATHS ${LLVM_TOOLS_BINARY_DIR} NO_DEFAULT_PATH)

# Use the discovered or configured build tools
# within beaker. Note that the native compiler is
# also used as the frontend to the native linker
# since that will guarantee that we get the C
# runtime in the output.
#
# TODO: Actually use the low-level tools and link
# only against the minimal Beaker runtime.
set(BEAKER_NATIVE_COMPILER ${CMAKE_C_COMPILER})
set(BEAKER_NATIVE_ARCHIVER ${CMAKE_AR})

# Compiler configuration
set(CMAKE_CXX_FLAGS "-Wall -std=c++1y")

if(NOT TARGET check)
  add_custom_target(check COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target test)
endif()

add_subdirectory(lingo)
add_subdirectory(beaker)
