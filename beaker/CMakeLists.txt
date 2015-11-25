# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Add LLVM preprocessor definitions.
add_definitions(${LLVM_DEFINITIONS})


# Generate a static configuration header in the
# build directory.
configure_file("config.hpp.in" "config.hpp")
include_directories(${CMAKE_CURRENT_BINARY_DIR})


# The Core Beaker library
add_library(beaker STATIC
  file.cpp
  line.cpp
  location.cpp
  symbol.cpp
  expr.cpp
  type.cpp
  decl.cpp
  specifier.cpp
  value.cpp
  print.cpp
  less.cpp
  convert.cpp
  error.cpp
  token.cpp
  lexer.cpp
  parser.cpp
  environment.cpp
  scope.cpp
  overload.cpp
  elaborator.cpp
  evaluator.cpp
  mangle.cpp
  generator.cpp
  job.cpp
)


# Libraries requried for executable targets.
set(libs
  beaker
  lingo
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${Boost_PROGRAM_OPTIONS_LIBRARIES}
  ${LLVM_LIBRARIES})


# The translator is performs front-end translation
# to an intermediate representation. This is analogous
# to cc1 for GCC/Clang, which translats C to assembly
# language.
add_executable(beaker-translate translate.cpp)
target_link_libraries(beaker-translate ${libs})


# The compiler is the main driver for compilation. 
# It takes a set of input files and produces linked
# outputs (programs, libraries, archives).
add_executable(beaker-compile compiler.cpp)
target_link_libraries(beaker-compile ${libs})


# The runtime interpreter executes a parsed beaker
# program without compiling to native code.
add_executable(beaker-interpret interpreter.cpp)
target_link_libraries(beaker-interpret ${libs})
