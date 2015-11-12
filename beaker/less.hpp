// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LESS_HPP
#define BEAKER_LESS_HPP

#include "prelude.hpp"


bool is_less(Type const*, Type const*);
bool is_less(Expr const*, Expr const*);


// A function object that invokes the is_less function.
template<typename T>
struct Less_fn
{
  bool operator()(T const* a, T const* b) const
  {
    return is_less(a, b);
  }
};


#endif
