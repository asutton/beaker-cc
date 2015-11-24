// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_FILE_HPP
#define BEAKER_FILE_HPP

#include "prelude.hpp"

#include <boost/filesystem.hpp>


namespace fs = boost::filesystem;


// A path represents the location of a file in the
// file system.
using Path = fs::path;


// A sequence of paths.
using Path_seq = std::vector<Path>;


// An input file.
class File
{
public:
  File(char const*);

  Path const&   path() const     { return path_; }
  String const& pathname() const { return path_.string(); }

private:
  Path path_;
};


#endif
