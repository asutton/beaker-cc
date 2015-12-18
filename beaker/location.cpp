// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "config.hpp"

#include "beaker/location.hpp"
#include "beaker/file.hpp"


std::ostream&
operator<<(std::ostream& os, Location const& l)
{
  if (l.file())
    os << l.file()->pathname() << ':';
  os << l.line() << ':' << l.column();
  return os;
}
