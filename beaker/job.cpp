// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "job.hpp"

#include <unistd.h>

#include <iostream>
#include <system_error>


// Execute the job. If the program cannot be executed,
// an exception is thrown.
void
Job::run()
{
  std::stringstream ss;
  ss << exec.string() << ' ';
  for (String const& arg : args)
    ss << arg << ' ';
  std::cout << ss.str() << '\n';
}
