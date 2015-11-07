# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Add LLVM preprocessor definitions.
add_definitions(${LLVM_DEFINITIONS})


# Create the beaker compiler.
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
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${LLVM_LIBRARIES}
  lingo)
