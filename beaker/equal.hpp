// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EQUAL_HPP
#define BEAKER_EQUAL_HPP

#include "prelude.hpp"


bool is_equal(Type const*, Type const*);


// A function object that invokes the is_equal function.
template<typename T>
struct Equal_fn
{
  bool operator()(T const* a, T const* b) const
  {
    return is_equal(a, b);
  }
};


#endif
