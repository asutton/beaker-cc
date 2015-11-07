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


    // FIXME: is this correct?
    // is the hash simply a hash of the pointers to their decl?
    std::size_t operator()(Record_type const* t)
    {
      std::size_t h = hash_ptr(t->decl());
      return h;
    }

    // network specific types
    std::size_t operator()(Table_type const* t)
    {
      // hash the member decls not the types
      std::size_t seed;
      boost::hash_combine(seed, t->key_fields());
      return seed;
    }

    std::size_t operator()(Flow_type const* t)
    {
      std::size_t seed;
      boost::hash_combine(seed, t->key_types());
      return seed;
    }

    // Currently no difference between port types
    std::size_t operator()(Port_type const* t)
    {
      std::size_t  h = hash_ptr(t);
      return h;
    }
  };

  std::size_t h = apply(t, Fn{});
  std::cout << "H: " << t << ' ' << h << '\n';
  return h;
}
