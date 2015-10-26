// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "location.hpp"
#include "file.hpp"


std::ostream& 
operator<<(std::ostream& os, Location const& l)
{
  if (l.file())
    os << l.file()->pathname() << ':';
  os << l.line() << ':' << l.column();
  return os;
}
