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
  array_value,
};


using Integer_value = int;
using Function_value = Function_decl const*;
using Reference_value = Value*;


// An array value is a sequence of values.
//
// FIXME: Memory allocated to this array is
// never freed. It probably should be.
struct Array_value
{
  Array_value(std::size_t n);
  Array_value(char const*, std::size_t n);

  std::string to_string() const;

  int    len;
  Value* data;
};


union Value_rep
{
  Value_rep() { }
  Value_rep(Integer_value z) : int_(z) { }
  Value_rep(Function_value f) : fn_(f) { }
  Value_rep(Reference_value r) : ref_(r) { }
  Value_rep(Array_value r) : arr_(r) { }
  ~Value_rep() { }


  Integer_value   int_;
  Function_value  fn_;
  Reference_value ref_;
  Array_value     arr_;
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

  Value(Array_value a)
    : k(array_value), r(a)
  { }

  Value(Value* v);

  ~Value() { }

  Value_kind kind() const { return k; }

  bool is_integer() const;
  bool is_function() const;
  bool is_reference() const;
  bool is_array() const;


  Integer_value get_integer() const;
  Function_value get_function() const;
  Reference_value get_reference() const;
  Array_value get_array() const;

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


// Returns true if the value is an integer.
inline bool
Value::is_integer() const
{
  return k == integer_value;
}


// Returns true if the value is a function.
inline bool
Value::is_function() const
{
  return k == function_value;
}


// Returns true if k is a reference.
inline bool
Value::is_reference() const
{
  return k == reference_value;
}


// Returns true if the value is a string.
inline bool
Value::is_array() const
{
  return k == array_value;
}


// Returns the integer value.
inline Integer_value
Value::get_integer() const
{
  assert(is_integer());
  return r.int_;
}


// Returns the function value.
inline Function_value
Value::get_function() const
{
  assert(is_function());
  return r.fn_;
}


// Get a pointer to the referred to value.
inline Reference_value
Value::get_reference() const
{
  assert(is_reference());
  return r.ref_;
}


// Returns the string value.
inline Array_value
Value::get_array() const
{
  assert(is_array());
  return r.arr_;
}


// -------------------------------------------------------------------------- //
// Array values

inline
Array_value::Array_value(std::size_t n)
  : len(n), data(new Value[n])
{ }


inline
Array_value::Array_value(char const* s, std::size_t n)
  : Array_value(n)
{
  std::copy(s, s + n, data);
}


// -------------------------------------------------------------------------- //
// Other types and functions


// A sequence of values.
using Value_seq = std::vector<Value>;


// Streaming
std::ostream& operator<<(std::ostream& os, Value const&);

#endif
