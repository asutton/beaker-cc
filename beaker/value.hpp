// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_VALUE_HPP
#define BEAKER_VALUE_HPP

// The value module represents compile-time or 
// interpreted values.
//
// TODO: Make a visitor for values.

#include "prelude.hpp"


struct Value;


enum Value_kind
{
  error_value,
  integer_value,
  function_value,
  reference_value,
};


using Integer_value = int;
using Function_value = Function_decl const*;
using Reference_value = Value*;


union Value_rep
{
  Value_rep() = default;
  Value_rep(Integer_value z) : int_(z) { }
  Value_rep(Function_value f) : fn_(f) { }
  Value_rep(Reference_value r) : ref_(r) { }

  Integer_value   int_;
  Function_value  fn_;
  Reference_value ref_;
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

  Value(Value* v);

  Value_kind kind() const { return k; }

  inline bool is_integer() const;
  inline bool is_function() const;
  inline bool is_reference() const;


  Integer_value get_integer() const;
  Function_value get_function() const;
  Reference_value get_reference() const;

  Value_kind k;
  Value_rep r;
};


// Construct a value reference. Not that reference
// chains are not permitted. That is, v shall not
// be a reference.
inline
Value::Value(Value* v)
  : k(reference_value), r(v)
{
  assert(!v->is_reference());
}


// Returns true if the value is an integer or
// a reference to an integer.
inline bool
Value::is_integer() const
{
  if (k == integer_value)
    return true;
  if (k == reference_value && r.ref_->k == integer_value)
    return true;
  return false;
}


// Returns true if the value is a function or a
// reference to a function.
inline bool
Value::is_function() const
{
  if (k == function_value)
    return true;
  if (k == reference_value && r.ref_->k == function_value)
    return true;
  return false;
}


// Returns true if k is a reference.
inline bool
Value::is_reference() const
{
  return k == reference_value;
}


// Retrieve the integer value. If the value is
// a reference to an integer, then return the
// dereferenced value.
inline Integer_value
Value::get_integer() const
{
  if (k == reference_value)
    return r.ref_->get_integer();
  assert(k == integer_value);
  return r.int_;
}


// Retrieve a function value. If the value is
// a reference to a function, then return the
// dereferenced value.
inline Function_value
Value::get_function() const
{
  if (k == reference_value)
    return r.ref_->get_function();
  assert(k == function_value);
  return r.fn_;
}


// Get the reference value. Note that this
// does not "unwind" the references since
// references to references are not permitted.
inline Reference_value
Value::get_reference() const
{
  assert(k == reference_value);
  return r.ref_;
}


// A sequence of values.
using Value_seq = std::vector<Value>;


// Streaming
std::ostream& operator<<(std::ostream& os, Value const&);

#endif
