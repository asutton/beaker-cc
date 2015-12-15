// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/type.hpp"
#include "beaker/decl.hpp"
#include "beaker/less.hpp"
#include "beaker/value.hpp"
#include "beaker/evaluator.hpp"

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


Record_decl*
Record_type::declaration() const
{
  return cast<Record_decl>(decl_);
}


Scope*
Record_type::scope() const
{
  return declaration()->scope();
}



// Returns the size of the array as an
// integer value.
int
Array_type::size() const
{
  Value v = evaluate(extent());
  return v.get_integer();
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


// Note that id types are not canonicalized.
// They don't need to be since they never
// escape elaboration.
Type const*
get_id_type(Symbol const* s)
{
  return new Id_type(s);
}


Type const*
get_boolean_type()
{
  static Boolean_type t;
  return &t;
}


Type const*
get_character_type()
{
  static Character_type t;
  return &t;
}


Type const*
get_integer_type(bool is_signed, int precision)
{
  static Integer_type st16(16);
  static Integer_type ut16(false,16);
  static Integer_type st32(32);
  static Integer_type ut32(false);
  static Integer_type st64(64);
  static Integer_type ut64(false,64);
  
  if (is_signed)
    switch (precision)
    {
        case 32:
            return &st32;
        case 16:
            return &st16;
        case 64:
            return &st64;
        default:
            throw std::runtime_error("No integer with precision " + std::to_string(precision));
    }
  else
    switch (precision)
    {
        case 32:
            return &ut32;
        case 16:
            return &ut16;
        case 64:
            return &ut64;
        default:
            throw std::runtime_error("No integer with precision " + std::to_string(precision));
    }
}


Type const*
get_float_type()
{
  static Float_type t;
  return &t;
}


Type const*
get_double_type()
{
  static Double_type t;
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


Type const*
get_array_type(Type const* t, Expr* n)
{
  static Type_set<Array_type> ts;
  auto ins = ts.emplace(t, n);
  return &*ins.first;
}


Type const*
get_block_type(Type const* t)
{
  static Type_set<Block_type> ts;
  auto ins = ts.emplace(t);
  return &*ins.first;
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
get_record_type(Record_decl* r)
{
  static Type_set<Record_type> ts;
  auto ins = ts.emplace(r);
  return &*ins.first;
}

// Gets the rank of a type
int
get_scalar_rank(Type const* t)
{
    // static types
    static Type const* b = get_boolean_type();
    static Type const* c = get_character_type();
    static Type const* ui16 = get_integer_type(false,16);
    static Type const* i16 = get_integer_type(16);
    static Type const* ui32 = get_integer_type(false);
    static Type const* i32 = get_integer_type();
    static Type const* ui64 = get_integer_type(false,64);
    static Type const* i64 = get_integer_type(64);
    static Type const* f = get_float_type();
    static Type const* d = get_double_type();
    
    // return rank
    if (t == b)
        return bool_rnk;
    if (t == c)
        return char_rnk;
    if (t == ui16)
        return uint16_rnk;
    if (t == i16)
        return int16_rnk;
    if (t == ui32)
        return uint32_rnk;
    if (t == i32)
        return int32_rnk;
    if (t == ui64)
        return uint64_rnk;
    if (t == i64)
        return int64_rnk;
    if (t == f)
        return float_rnk;
    if (t == d)
        return double_rnk;
    
    // default case
    return default_rnk;
}

bool
is_derived(Type const* derived, Type const* base){
  if (Record_type const* d = as<Record_type>(derived))  {
    if (Record_type const* b = as<Record_type>(base)) {
      // If D and B are the same return true
      Record_decl * der = d->declaration();
      do {
        if (der == b->declaration())
          return true;
        der = der->base()->declaration();
      } while (der);
    }
  }
  return false;
}
