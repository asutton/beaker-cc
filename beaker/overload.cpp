// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/overload.hpp"
#include "beaker/type.hpp"
#include "beaker/decl.hpp"

#include <algorithm>


Symbol const*
Overload::name() const
{
  return front()->name();
}


namespace
{

// Return true if two functions have the same
// parameter types.
bool
same_parameters(Function_decl const* f1, Function_decl const* f2)
{
  Function_type const* t1 = f1->type();
  Function_type const* t2 = f2->type();
  return t1->parameter_types() == t2->parameter_types();
}


} // namespace


// Returns true if d1 and d2 can be added to the
// same overload set.
//
// TODO: What other criteria need to be considered when
// determining if functions can be overloaded.
//
// TODO: Return an error code indicating a reason for
// failure. That should be diagnosed by the elaborator.
bool
can_overload(Decl const* d1, Decl const* d2)
{
  // Only functions can be overloaded.
  if (!is<Function_decl>(d1))
    return false;
  if (!is<Function_decl>(d2))
    return false;

  // Two functions whose types differ only in the
  // return type cannot be overloaded.
  Function_decl const* f1 = cast<Function_decl>(d1);
  Function_decl const* f2 = cast<Function_decl>(d2);
  if (same_parameters(f1, f2) && f1->return_type() != f2->return_type())
    return false;

  return true;
}

