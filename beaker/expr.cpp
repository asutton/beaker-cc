// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "expr.hpp"
#include "type.hpp"
#include "decl.hpp"

#include <iostream>


Decl_expr::Decl_expr(Type const* t, Decl* d)
  : Id_expr(t, d->name()), decl(d)
{ }


Symbol const*
Decl_expr::name() const
{
  return decl->name();
}

Function_type const*
Lambda_expr::type() const
{
  return cast<Function_type>(type_);
}

Type const*
Lambda_expr::return_type() const
{
  std::cout << "In return type" << std::endl;
  return type()->return_type();
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


// Return true if e is an expression that be used
// as the target of a function call.
bool
is_callable(Expr const* e)
{
  // An overload is callable iff its members
  // are functions.
  if (Overload_expr const* ovl = as<Overload_expr>(e)) {
    Decl const* d = ovl->declarations().front();
    return is<Function_type>(d->type());
  }

  // An unresolved dot expression is callable if
  // its member is callable.
  else if (Dot_expr const* dot = as<Dot_expr>(e))
    return is_callable(dot->member());

  // Objects with function type are callable.
  return is<Function_type>(e->type());
}
