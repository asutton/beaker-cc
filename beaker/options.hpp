// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_OPTIONS_HPP
#define BEAKER_OPTIONS_HPP

// Provides support for parsing and using command
// line options.
//
// FIXME: Find a better solution to program 
// configuration.


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop


namespace po = boost::program_options;


using Arguments = po::variables_map;


#endif
