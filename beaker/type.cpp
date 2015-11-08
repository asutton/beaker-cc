// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "type.hpp"
#include "decl.hpp"
#include "less.hpp"

#include <set>


// Return a reference type for this type.
Type const*
Type::ref() const
{
  return get_reference_type(this);
}


// Return the non-reference type for this type.
Type const*
Type::nonref() const
{
  return this;
}


Type const*
Reference_type::ref() const
{
  return this;
}


Type const*
Reference_type::nonref() const
{
  return first;
}


Record_decl const*
Record_type::decl() const
{
  return cast<Record_decl>(decl_);
}


// -------------------------------------------------------------------------- //
// Type accessors

template<typename T>
struct Type_less
{
  bool operator()(T const& a, T const& b) const
  {
    return is_less(&a, &b);
  }
};


template<typename T>
using Type_set = std::set<T, Type_less<T>>;


Type const*
get_id_type(Symbol const* s)
{
  static Type_set<Id_type> fn;
  auto ins = fn.emplace(s);
  return &*ins.first;
}

Type const*
get_boolean_type()
{
  static Boolean_type t;
  return &t;
}


Type const*
get_integer_type()
{
  static Integer_type t;
  return &t;
}


Type const*
get_function_type(Type_seq const& t, Type const* r)
{
  static Type_set<Function_type> fn;
  auto ins = fn.emplace(t, r);
  return &*ins.first;
}


Type const*
get_function_type(Decl_seq const& d, Type const* r)
{
  Type_seq t;
  t.reserve(d.size());
  for (Decl* d1 : d)
    t.push_back(d1->type());
  return get_function_type(t, r);
}


// FIXME: Don't allow references to non-object types.
Type const*
get_reference_type(Type const* t)
{
  static Type_set<Reference_type> ts;
  auto ins = ts.emplace(t);
  return &*ins.first;
}


Type const*
get_record_type(Record_decl const* r)
{
  static Type_set<Record_type> ts;
  auto ins = ts.emplace(r);
  return &*ins.first;
}
