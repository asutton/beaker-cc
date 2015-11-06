// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "equal.hpp"
#include "type.hpp"

#include <algorithm>


template<typename T>
inline bool
is_equal(std::vector<T*> const& a, std::vector<T*> const& b)
{
  auto cmp = [](T const* x, T const* y) { return is_equal(x, y); };
  return std::equal(a.begin(), a.end(), b.begin(), b.end(), cmp);
}


inline bool
is_equal(Function_type const* a, Function_type const* b)
{
  return is_equal(a->parameter_types(), b->parameter_types()) 
      && is_equal(a->return_type(), b->return_type());
}


// Two record types are the same if they point to the same
// record declaration
inline bool
is_equal(Record_type const* a, Record_type const* b)
{
  return a->decl() == b->decl();
}


bool
is_equal(Type const* a, Type const* b)
{
  struct Fn
  {
    Type const* b;

    bool operator()(Boolean_type const* a)
    {
      return true;
    }

    bool operator()(Integer_type const* a)
    {
      return true;
    }

    bool operator()(Function_type const* a)
    {
      return is_equal(a, cast<Function_type>(b));
    }

    bool operator()(Record_type const* a)
    {
      return is_equal(a, cast<Record_type>(b));
    }
  };

  if (typeid(*a) != typeid(*b))
    return false;
  return apply(a, Fn{b});
}