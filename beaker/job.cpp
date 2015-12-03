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
  // FIXME: If the error is produced as a result of the
  // system() call, then diagnose that here. Otherwise,
  // if the program returned non-zero, then assume that
  // it's errors have been diagnosed.
  int err = system(ss.str().c_str());
  if (err)
    return false;
  return true;
}
