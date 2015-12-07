// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/decl.hpp"


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


// Returns the index of this field within its record
// declaration.
int
Field_decl::index() const
{
  Decl_seq const& f = context()->fields();
  for (std::size_t i = 0; i < f.size(); ++i)
    if (f[i] == this)
      return i;
  lingo_unreachable();
}


// Returns the record's base class type.
Record_type const*
Record_decl::base() const
{
  return cast<Record_type>(base_);
}


// Returns true when the declaration is declared as
// a reference to an object.
bool
is_reference(Decl const* d)
{
  return is<Reference_type>(d->type());
}
