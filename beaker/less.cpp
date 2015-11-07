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


// A record type is less if the decl they point to are less
// All record declarations are unique and thus all record types
// depend on those record declarations to determine if they are less/equal
inline bool
is_less(Record_type const* a, Record_type const* b)
{
  return a->decl() < b->decl();
}


// Two table types are equal if all of their key fields
// are equal
inline bool
is_less(Table_type const* a, Table_type const* b)
{
  Decl_seq const& a_fields = a->key_fields();
  Decl_seq const& b_fields = b->key_fields();

  auto cmp = [](Decl const* x, Decl const* y) { return is_less(x->type(), y->type()); };
  return std::lexicographical_compare(a_fields.begin(), a_fields.end(), 
                                      b_fields.begin(), b_fields.end(), cmp);
}


// Two flow types are equal if each of their key types
// are equal
inline bool
is_less(Flow_type const* a, Flow_type const* b)
{
  return is_less(a->key_types(), b->key_types());
}


inline bool
is_less(Reference_type const* a, Reference_type const* b)
{
  return is_less(a->first, b->first);
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

    bool operator()(Record_type const* a)
    {
      return is_less(a, cast<Record_type>(b));
    }

    bool operator()(Reference_type const* a)
    {
      return is_less(a, cast<Reference_type>(b));
    }

    // network specific types
    bool operator()(Table_type const* a)
    {
      return is_less(a, cast<Table_type>(b));
    }

    bool operator()(Flow_type const* a)
    {
      return is_less(a, cast<Flow_type>(b));
    }

    // Currently no difference between port types
    bool operator()(Port_type const* a)
    {
      return false;
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