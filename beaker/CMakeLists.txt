# Copyright (c) 2015 Andrew Sutton
# All rights reserved

# Generate a static configuration header in the
# build directory.
configure_file("config.hpp.in" "config.hpp")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Add the core Beaker library.
add_library(beaker
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
target_compile_definitions(beaker PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(
  beaker
    PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
      ${Boost_INCLUDE_DIRS}
      ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(
  beaker
    PUBLIC
      lingo
      ${Boost_LIBRARIES}
      ${LLVM_LIBRARIES}
)

# The compiler is the main driver for compilation.
# It takes a set of input files and produces linked
# outputs (programs, libraries, archives).
add_executable(beaker-compile driver.cpp compiler.cpp)
target_link_libraries(beaker-compile beaker)


# The runtime interpreter executes a parsed beaker
# program without compiling to native code.
add_executable(beaker-interpret interpreter.cpp)
target_link_libraries(beaker-interpret beaker)
