// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "expr.hpp"
#include "type.hpp"
#include "decl.hpp"


Symbol const*
Decl_expr::name() const
{
  return decl->name();
}
