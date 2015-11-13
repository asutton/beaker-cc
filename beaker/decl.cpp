// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "decl.hpp"
#include "type.hpp"


Function_type const*
Function_decl::type() const
{
  return cast<Function_type>(Decl::type());
}


Type const*
Function_decl::return_type() const
{
  return type()->return_type();
}


// Returns true when the declaration is declared as
// a reference to an object.
bool
is_reference(Decl const* d)
{
  return is<Reference_type>(d->type());
}
