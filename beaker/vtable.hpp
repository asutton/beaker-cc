// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_VTABLE_HPP
#define BEAKER_VTABLE_HPP

// The vtable module provides support for constructing
// and maintaint virtual table methods.

#include "prelude.hpp"


// A virtual is essentially a collection of function
// declarations. 
struct Virtual_table : std::vector<Decl const*>
{
};

#endif
