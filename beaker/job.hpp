// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_JOB_HPP
#define BEAKER_JOB_HPP

// Represents jobs that are executed as part of the
// compilation process.

#include "file.hpp"

#include "lingo/string.hpp"


// A job is a program that is executed to translate an
// input file into an output file. A job corresponds
// a subprocess executed by the compiler.
struct Job
{
  Job(Path const& p, String_seq const& args)
    : exec(p), args(args)
  { }

  bool run();

  Path       exec;
  String_seq args;
};


// The job list is the sequence of jobs needed to fully
// execute the compilation process.
using Job_seq = std::vector<Job>;


#endif
