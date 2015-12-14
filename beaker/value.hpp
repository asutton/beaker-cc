// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_VALUE_HPP
#define BEAKER_VALUE_HPP

// The value module represents compile-time or
// interpreted values.
//
// TODO: Make a visitor for values.

#include <beaker/prelude.hpp>


struct Value;


enum Value_kind
{
  error_value,
  integer_value,
  function_value,
  reference_value,
  array_value,
  tuple_value,
};


// An empty representation of an error state.
struct Error_value { };


using Integer_value = int;
using Function_value = Function_decl const*;
using Reference_value = Value*;


// The common structure of array and tuple
// values.
struct Aggregate_value
{
  Aggregate_value(std::size_t n);
  Aggregate_value(char const*, std::size_t n);

  std::size_t len;
  Value*      data;
};


// An array value is a sequence of values of the
// same kind.
//
// FIXME: Memory allocated to this array is
// never freed. It probably should be.
struct Array_value : Aggregate_value
{
  using Aggregate_value::Aggregate_value;

  std::string get_string() const;
};


// A tuple value is a sequence of values of
// different kind. This is used to represent
// record values, except that we don't care
// about field names at this point.
//
// This is effectively the same as an array
// but with weaker "type" guarantees.
struct Tuple_value : Aggregate_value
{
  using Aggregate_value::Aggregate_value;
};


union Value_rep
{
  Value_rep() : err_() { }
  Value_rep(Integer_value z) : int_(z) { }
  Value_rep(Function_value f) : fn_(f) { }
  Value_rep(Reference_value r) : ref_(r) { }
  Value_rep(Array_value a) : arr_(a) { }
  Value_rep(Tuple_value t) : tup_(t) { }
  ~Value_rep() { }


  Error_value     err_;
  Integer_value   int_;
  Function_value  fn_;
  Reference_value ref_;
  Array_value     arr_;
  Tuple_value     tup_;
};


// Represents a compile time value.
struct Value
{
  struct Visitor;
  struct Mutator;

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

  Value(Tuple_value a)
    : k(tuple_value), r(a)
  { }

  Value(Value* v);

  ~Value() { }

  void accept(Visitor&) const;
  void accept(Mutator&);

  Value_kind kind() const { return k; }
  bool is_error() const;
  bool is_integer() const;
  bool is_function() const;
  bool is_reference() const;
  bool is_array() const;
  bool is_tuple() const;

  Integer_value get_integer() const;
  Function_value get_function() const;
  Reference_value get_reference() const;
  Array_value get_array() const;
  Tuple_value get_tuple() const;

  Value_kind k;
  Value_rep r;
};


// The non-modifying visitor.
struct Value::Visitor
{
  virtual void visit(Error_value const&) = 0;
  virtual void visit(Integer_value const&) = 0;
  virtual void visit(Function_value const&) = 0;
  virtual void visit(Reference_value const&) = 0;
  virtual void visit(Array_value const&) = 0;
  virtual void visit(Tuple_value const&) = 0;
};


// The modifying visitor
struct Value::Mutator
{
  virtual void visit(Error_value&) = 0;
  virtual void visit(Integer_value&) = 0;
  virtual void visit(Function_value&) = 0;
  virtual void visit(Reference_value&) = 0;
  virtual void visit(Array_value&) = 0;
  virtual void visit(Tuple_value&) = 0;
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


// Returns true if the value is an error.
inline bool
Value::is_error() const
{
  return k == error_value;
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


// Returns true if the value is an array.
inline bool
Value::is_array() const
{
  return k == array_value;
}


// Returns true if the value is a tuple.
inline bool
Value::is_tuple() const
{
  return k == tuple_value;
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


// Returns the array value.
inline Array_value
Value::get_array() const
{
  assert(is_array());
  return r.arr_;
}


// Returns the array value.
inline Tuple_value
Value::get_tuple() const
{
  assert(is_tuple());
  return r.tup_;
}


inline void
Value::accept(Visitor& v) const
{
  switch (k) {
    case error_value: return v.visit(r.err_);
    case integer_value: return v.visit(r.int_);
    case function_value: return v.visit(r.fn_);
    case reference_value: return v.visit(r.ref_);
    case array_value: return v.visit(r.arr_);
    case tuple_value: return v.visit(r.tup_);
  }
}


inline void
Value::accept(Mutator& v)
{
  switch (k) {
    case error_value: return v.visit(r.err_);
    case integer_value: return v.visit(r.int_);
    case function_value: return v.visit(r.fn_);
    case reference_value: return v.visit(r.ref_);
    case array_value: return v.visit(r.arr_);
    case tuple_value: return v.visit(r.tup_);
  }
}


// -------------------------------------------------------------------------- //
// Generic visitors

template<typename F, typename T>
struct Generic_value_visitor : Value::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_value_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }

  void visit(Error_value const& v) { this->invoke(v); };
  void visit(Integer_value const& v) { this->invoke(v); };
  void visit(Function_value const& v) { this->invoke(v); };
  void visit(Reference_value const& v) { this->invoke(v); };
  void visit(Array_value const& v) { this->invoke(v); };
  void visit(Tuple_value const& v) { this->invoke(v); };
};


template<typename F, typename T = typename std::result_of<F(Error_value const&)>::type>
inline T
apply(Value const& v, F fn)
{
  Generic_value_visitor<F, T> vis(fn);
  return accept(v, vis);
}


template<typename F, typename T>
struct Generic_value_mutator : Value::Mutator, lingo::Generic_mutator<F, T>
{
  Generic_value_mutator(F fn)
    : lingo::Generic_mutator<F, T>(fn)
  { }

  void visit(Error_value& v) { this->invoke(v); };
  void visit(Integer_value& v) { this->invoke(v); };
  void visit(Function_value& v) { this->invoke(v); };
  void visit(Reference_value& v) { this->invoke(v); };
  void visit(Array_value& v) { this->invoke(v); };
  void visit(Tuple_value& v) { this->invoke(v); };
};


template<typename F, typename T = typename std::result_of<F(Error_value&)>::type>
inline T
apply(Value& v, F fn)
{
  Generic_value_mutator<F, T> vis(fn);
  return accept(v, vis);
}



// -------------------------------------------------------------------------- //
// Aggregate values

inline
Aggregate_value::Aggregate_value(std::size_t n)
  : len(n), data(new Value[n])
{ }


inline
Aggregate_value::Aggregate_value(char const* s, std::size_t n)
  : Aggregate_value(n)
{
  std::copy(s, s + n, data);
}


// -------------------------------------------------------------------------- //
// Intrinsic behaviors

void zero_init(Value&);


// -------------------------------------------------------------------------- //
// Other types and functions


// A sequence of values.
using Value_seq = std::vector<Value>;


// -------------------------------------------------------------------------- //
// Printing
//
// TODO: Move this to the print module?

std::ostream& operator<<(std::ostream&, Value const&);

#endif
