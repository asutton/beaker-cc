// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "less.hpp"
#include "type.hpp"

#include <algorithm>
#include <typeindex>


// TODO: Rewrite using lingo node concepts.


template<typename T>
inline bool
is_less(std::vector<T> const& a, std::vector<T> const& b)
{
  auto cmp = [](T const& x, T const& y) { return is_less(x, y); };
  return std::lexicographical_compare(a.begin(), a.end(),
                                      b.begin(), b.end(), cmp);
}


inline bool
is_less(Function_type const* a, Function_type const* b)
{
  if (is_less(a->parameter_types(), b->parameter_types()))
    return true;
  if (is_less(b->parameter_types(), a->parameter_types()))
    return false;
  return is_less(a->return_type(), b->return_type());
}


inline bool
is_less(Reference_type const* a, Reference_type const* b)
{
  return is_less(a->first, b->first);
}


inline bool
is_less(Record_type const* a, Record_type const* b)
{
  std::less<void const*> cmp;
  return cmp(a->decl(), b->decl());
}


bool
is_less(Type const* a, Type const* b)
{
  struct Fn
  {
    Type const* b;

    bool operator()(Boolean_type const* a) { return false; }
    bool operator()(Integer_type const* a) { return false; }

    bool operator()(Function_type const* a)
    {
      return is_less(a, cast<Function_type>(b));
    }

    bool operator()(Reference_type const* a)
    {
      return is_less(a, cast<Reference_type>(b));
    }

    bool operator()(Record_type const* a)
    {
      return is_less(a, cast<Record_type>(b));
    }
  };

  std::type_index t1 = typeid(*a);
  std::type_index t2 = typeid(*b);
  if (t1 < t2)
    return true;
  if (t2 < t1)
    return true;
  return apply(a, Fn{b});
}
