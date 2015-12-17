// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/file.hpp"
#include "beaker/error.hpp"


extern int compiler_main(int, char*[]);


int
main(int argc, char* argv[])
{
  return compiler_main(argc, argv);
}
