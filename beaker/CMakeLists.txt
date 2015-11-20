# Copyright (c) 2015 Andrew Sutton
# All rights reserved

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
)
target_compile_definitions(beaker PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(
  beaker
    PUBLIC
      ${Boost_INCLUDE_DIRS}
      ${LLVM_INCLUDE_DIRS}
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)
target_link_libraries(
  beaker
    PUBLIC
      lingo
      ${Boost_LIBRARIES}
      ${LLVM_LIBRARIES}
)

# Create the beaker compiler.
add_executable(beaker-compile compiler.cpp)
target_link_libraries(beaker-compile beaker)

# Create the beaker runtime interpreter.
add_executable(beaker-interpret interpreter.cpp)
target_link_libraries(beaker-interpret beaker)
