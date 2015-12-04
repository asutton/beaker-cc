// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include <iostream>
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


std::vector<int>
Field_decl::index() const
{
  std::vector<int> path;

  // Find the index of this field within the record.
  //
  // FIXME: Why isn't this set in the constructor or
  // during elaboration.
  Decl_seq const& f = context()->fields();
  for (std::size_t i = 0; i < f.size(); ++i)
    if (f[i] == this) {
      path.push_back(i);
      return path;
    }


  Record_decl const* decl;
  while (decl->base_decl != nullptr) {
    decl = decl->base_decl;
    path.push_back(0);
    Decl_seq const& f = decl->fields();
    for (std::size_t i = 0; i < f.size(); ++i)
      if (f[i] == this) {
          path.push_back(i);
          return path;
      }
  }
  
  // If the last element is -1, it does not exist.
  path.push_back(-1);
  return path;
}


// Returns true when the declaration is declared as
// a reference to an object.
bool
is_reference(Decl const* d)
{
  return is<Reference_type>(d->type());
}
