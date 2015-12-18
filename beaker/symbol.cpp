// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "config.hpp"

#include "beaker/symbol.hpp"


std::ostream&
operator<<(std::ostream& os, Symbol const& sym)
{
  return os << sym.spelling();
}
