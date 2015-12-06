// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/value.hpp"
#include "beaker/decl.hpp"

#include <iostream>


// Return a string value for the arary. This is
// needed for any transformation to narrow string
// literals in the evaluation character set.
std::string
Array_value::get_string() const
{
  std::string str(len, '\0');
  std::transform(data, data + len, str.begin(), [](Value const& v) -> char {
    return v.get_integer();
  });
  return str;
}


// -------------------------------------------------------------------------- //
// Printing

// FIXME: The functions below vary only in their
// enclosing characters. Make a single function.

inline void
print(std::ostream& os, Array_value const& v)
{
  os << '[';
  Value const* p = v.data;
  Value const* q = p + v.len;
  while (p != q) {
    os << *p;
    if (p + 1 != q)
      os << ',';
    ++p;
  }
  os << ']';
}


inline void
print(std::ostream& os, Tuple_value const& v)
{
  os << '{';
  Value const* p = v.data;
  Value const* q = p + v.len;
  while (p != q) {
    os << *p;
    if (p + 1 != q)
      os << ',';
    ++p;
  }
  os << '}';
}


std::ostream&
operator<<(std::ostream& os, Value const& v)
{
  struct Fn
  {
    std::ostream& os;

    void operator()(Error_value const& v) { os << "<error>"; }
    void operator()(Integer_value const& v) { os << v; };
    void operator()(Function_value const& v) { os << v->name()->spelling(); };
    void operator()(Reference_value const& v) { os << *v << '@' << (void*)v; };
    void operator()(Array_value const& v) { print(os, v); }
    void operator()(Tuple_value const& v) { print(os, v); }
  };

  apply(v, Fn{os});
  return os;
}


// -------------------------------------------------------------------------- //
// Zero initialization


// Set to 0.
inline void
zero_init(Integer_value& v)
{
  v = 0;
}


// Set to null.
//
// TODO: Can a function value ever be zero initialized?
// For example, is this valid?
//
//    var f : () -> int;
//
// This would effectively decare a default initialized
// function pointer. There are arguments for and against.
inline void
zero_init(Function_value& v)
{
  throw std::runtime_error("zero initialization of function");
}


// This is most definitely not defined.
inline void
zero_init(Reference_value& v)
{
  throw std::runtime_error("zero initialization of reference");
}


// Recursively zero initialize the aggregate.
void
zero_init(Aggregate_value& v)
{
  for (std::size_t i = 0; i < v.len; ++i)
    zero_init(v.data[i]);
}

// Zero initialzie the value.
void
zero_init(Value& v)
{
  struct Fn
  {
    void operator()(Error_value& v) { };
    void operator()(Integer_value& v) { zero_init(v); };
    void operator()(Function_value& v) { zero_init(v); }
    void operator()(Reference_value& v) { zero_init(v); }
    void operator()(Aggregate_value& v) { zero_init(v); };
  };
  apply(v, Fn{});
}

// -------------------------------------------------------------------------- //
// Trivial initialization

inline void
trivial_init(Integer_value& v)
{

}


// Set to null.
//
// TODO: Can a function value ever be zero initialized?
// For example, is this valid?
//
//    var f : () -> int;
//
// This would effectively decare a default initialized
// function pointer. There are arguments for and against.
inline void
trivial_init(Function_value& v)
{
  throw std::runtime_error("zero initialization of function");
}


// This is most definitely not defined.
inline void
trivial_init(Reference_value& v)
{
  throw std::runtime_error("zero initialization of reference");
}


// Recursively zero initialize the aggregate.
void
trivial_init(Aggregate_value& v)
{
  for (std::size_t i = 0; i < v.len; ++i)
    trivial_init(v.data[i]);
}

// Zero initialzie the value.
void
trivial_init(Value& v)
{
  struct Fn
  {
    void operator()(Error_value& v) { };
    void operator()(Integer_value& v) { trivial_init(v); };
    void operator()(Function_value& v) { trivial_init(v); }
    void operator()(Reference_value& v) { trivial_init(v); }
    void operator()(Aggregate_value& v) { trivial_init(v); };
  };
  apply(v, Fn{});
}
