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

Record_decl*
Field_expr::record() const
{
  Record_type const* r = cast<Record_type>(container()->type());
  return r->declaration();
}


Record_decl*
Method_expr::record() const
{
  Record_type const* r = cast<Record_type>(container()->type());
  return r->declaration();
}
