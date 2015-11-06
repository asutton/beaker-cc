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


// Two table types are equal if all of their key fields
// are equal
inline bool
is_equal(Table_type const* a, Table_type const* b)
{
  Decl_seq const& a_fields = a->key_fields();
  Decl_seq const& b_fields = b->key_fields();

  auto cmp = [](Decl const* x, Decl const* y) { return is_equal(x->type(), y->type()); };
  return std::equal(a_fields.begin(), a_fields.end(), b_fields.begin(), b_fields.end(), cmp);
}


// Two flow types are equal if each of their key types
// are equal
inline bool
is_equal(Flow_type const* a, Flow_type const* b)
{
  return is_equal(a->key_types(), b->key_types());
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

    // network specific types
    bool operator()(Table_type const* a)
    {
      return is_equal(a, cast<Table_type>(b));
    }

    bool operator()(Flow_type const* a)
    {
      return is_equal(a, cast<Flow_type>(b));
    }

    // Currently no difference between port types
    bool operator()(Port_type const* a)
    {
      return true;
    }
  };

  if (typeid(*a) != typeid(*b))
    return false;
  return apply(a, Fn{b});
}