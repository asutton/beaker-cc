// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LOCATION_HPP
#define BEAKER_LOCATION_HPP

#include <iosfwd>
#include <unordered_map>


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


// The location map associates terms of the
// language with their location in source code.
// Note that types do not have a source code
// location since they are uniqued.
//
// TODO: Use this to also determine the
// end of a term.
struct Location_map : std::unordered_map<void const*, Location>
{
  using std::unordered_map<void const*, Location>::unordered_map;

  Location get(void const*) const;
};


inline Location
Location_map::get(void const* p) const
{
  auto iter = find(p);
  if (iter != end())
    return iter->second;
  else
    return {};
}


// Streaming
std::ostream& operator<<(std::ostream&, Location const&);


#endif
