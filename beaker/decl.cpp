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


Record_decl*
Record_decl::base_declaration() const
{
  if (base_)
    return base()->declaration();
  else
    return nullptr;
}


// Returns true if the record has no members.
bool
Record_decl::is_empty() const
{
  // A polymorphic type is not an empty class.
  if (is_polymorphic())
    return false;

  // If a base class is non-empty, then this class
  // is non-empty.
  if (Record_decl const* b = base_declaration())
    if (b->is_empty())
      return false;
  
  // An empty base class has no fields.
  return fields_.empty();
}


// Returns true when the declaration is declared as
// a reference to an object.
bool
is_reference(Decl const* d)
{
  return is<Reference_type>(d->type());
}
