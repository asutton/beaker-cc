// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_FILE_HPP
#define BEAKER_FILE_HPP

#include "string.hpp"

#include <boost/filesystem.hpp>


using Path = boost::filesystem::path;


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
