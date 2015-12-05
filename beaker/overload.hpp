// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_OVERLOAD_HPP
#define BEAKER_OVERLOAD_HPP

#include <beaker/prelude.hpp>


// Represents a set of overloaded declarations. All
// declarations have the same name, scope, and kind,
// but different types.
//
// Note that an overload set is never empty.
struct Overload : std::vector<Decl*>
{
  Symbol const* name() const;
};


bool can_overload(Decl const*, Decl const*);

#endif
