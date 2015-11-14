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


Field_decl*
Field_expr::field() const
{
  return cast<Field_decl>(var);
}


// Returns the index of the field within its
// record defintition.
int
Field_expr::index() const
{
  return field()->index();
}


Record_decl*
Method_expr::record() const
{
  Record_type const* r = cast<Record_type>(container()->type());
  return r->declaration();
}


Method_decl*
Method_expr::method() const
{
  return cast<Method_decl>(fn);
}
