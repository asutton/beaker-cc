set(CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall")

add_library(metal-cxx STATIC
  string.cpp
  location.cpp
  format.cpp
  debug.cpp
  nodes.cpp
  ast.cpp
  name.cpp
  type.cpp
  expr.cpp
  decl.cpp
  stmt.cpp
)