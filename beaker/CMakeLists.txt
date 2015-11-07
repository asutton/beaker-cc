# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Add LLVM preprocessor definitions.
add_definitions(${LLVM_DEFINITIONS})


# The Core Beaker library
add_library(beaker STATIC
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
  generator.cpp
)



# Libraries requried for executable targets.
set(libs
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${LLVM_LIBRARIES}
  lingo
  beaker)


# Create the beaker compiler.
add_executable(beaker-compile compiler.cpp)
target_link_libraries(beaker-compile ${libs})

# Create the beaker runtime interpreter.
add_executable(beaker-interpret interpreter.cpp)
target_link_libraries(beaker-interpret ${libs})