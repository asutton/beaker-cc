// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "file.hpp"
#include "error.hpp"


// The name of the translate command. This is global
// because translate_main is going to depend on it.
Path translator;


extern int translator_main(int, char*[]);
extern int compiler_main(int, char*[]);


int
main(int argc, char* argv[])
{
  // Get the name of the executed program.
  Path arg0 = argv[0];
  Path command = arg0.filename();
  
  // Build the equivalent name for the translator.
  // Note that this assumes that the compiler and
  // translator binaries are installed in the same
  // directory (as they should be).
  translator = arg0;
  translator.remove_filename();
  translator /= "beaker-translate";

  // Dispatch to the appropriate program.
  if (command == "beaker-translate")
    return translator_main(argc, argv);
  else
    return compiler_main(argc, argv);

  return 0;
}
