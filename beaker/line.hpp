// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LINE_HPP
#define BEAKER_LINE_HPP

#include <map>


// A line is a view into a string buffer.
struct Line
{
  Line(int n, char const* f, char const* l)
    : num_(n), first_(f), last_(l)
  { }

  int number() const { return num_; }

  char const* begin() const { return first_; }
  char const* end() const   { return last_; }

  int         num_;
  char const* first_;
  char const* last_;
};


// A line map associates the offset in a file
// with its corresponding line.
struct Line_map : std::map<int, Line>
{
  Line const& line(int n) const;
};


// Return the line in which the offset appears.
inline Line const& 
Line_map::line(int n) const
{
  return (--upper_bound(n))->second;
}


#endif
