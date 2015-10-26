// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_VALUE_HPP
#define BEAKER_VALUE_HPP

#include "prelude.hpp"

#include <vector>


enum Value_kind
{
  error_value,
  integer_value,
  function_value
};


using Integer_value = int;
using Function_value = Function_decl const*;


union Value_rep
{
  Value_rep() = default;
  Value_rep(Integer_value z) : z(z) { }
  Value_rep(Function_value f) : f(f) { }

  Integer_value  z;
  Function_value f;
};


// Represents a compile time value.
struct Value
{
  Value()
    : k(error_value), r()
  { }

  Value(Integer_value n)
    : k(integer_value), r(n)
  { }

  Value(Function_value f)
    : k(function_value), r(f)
  { }

  Value_kind k;
  Value_rep r;
};


using Value_seq = std::vector<Value>;


#endif
