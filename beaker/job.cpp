// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "job.hpp"

#include <cstdlib>

#include <iostream>
#include <system_error>


// Execute the job. If the program cannot be executed,
// an exception is thrown.
bool
Job::run()
{
  // Build the command string.
  std::stringstream ss;
  ss << exec.string() << ' ';
  for (String const& arg : args)
    ss << arg << ' ';

  // Actually run the command.
  //
  // TODO: Improve diagnostics!
  int err = system(ss.str().c_str());
  if (err) {
    std::cout << "error exeucting " << exec.string() << '\n';
    return false;
  }
  return true;
}
