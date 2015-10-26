// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LOCATION_HPP
#define BEAKER_LOCATION_HPP

#include <iosfwd>


class File;


// A location in source code.
//
// FIXME: This should refer into some kind of lexical
// environment so that we can minimize the size of
// this object (because there will be lots of them).
struct Location
{
public:
  Location()
    : file_(nullptr), line_(0), col_(0)
  { }
  
  Location(File const* f, int l, int o)
    : file_(f), line_(l), col_(o)
  { }

  File const* file() const   { return file_; }
  int         line() const   { return line_; }
  int         column() const { return col_; }

  File const* file_;
  int         line_;
  int         col_;
};


std::ostream& operator<<(std::ostream&, Location const&);


#endif
