// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "hash.hpp"
#include "type.hpp"

#include <iostream>


std::size_t 
hash_value(Type const* t)
{
  struct Fn
  {
    std::hash<void const*> hash_ptr;

    std::size_t operator()(Boolean_type const* t)
    {
      std::size_t h = hash_ptr(t);
      std::cout << "B: " << h << '\n';
      return h;
    }

    std::size_t operator()(Integer_type const* t)
    {
      std::size_t h = hash_ptr(t);
      std::cout << "Z: " << h << '\n';
      return h;
    }

    std::size_t operator()(Function_type const* t)
    {
      std::size_t seed;
      boost::hash_combine(seed, t->parameter_types());
      boost::hash_combine(seed, t->return_type());
      return seed;
    }
  };

  std::size_t h = apply(t, Fn{});
  std::cout << "H: " << t << ' ' << h << '\n';
  return h;
}
