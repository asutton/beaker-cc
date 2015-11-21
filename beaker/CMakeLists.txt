# Copyright (c) 2015 Andrew Sutton
# All rights reserved


# Add LLVM preprocessor definitions.
add_definitions(${LLVM_DEFINITIONS})


# The Core Beaker library
add_library(beaker STATIC
  file.cpp
  line.cpp
  location.cpp
  symbol.cpp
  directive.cpp
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



# Libraries requried for executable targets.
set(libs
  beaker
  lingo
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${LLVM_LIBRARIES})


# Create the beaker compiler.
add_executable(beaker-compile compiler.cpp)
target_link_libraries(beaker-compile ${libs})

# Create the beaker runtime interpreter.
add_executable(beaker-interpret interpreter.cpp)
target_link_libraries(beaker-interpret ${libs})
