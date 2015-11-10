// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "value.hpp"
#include "decl.hpp"


// Return a string value for the arary.
std::string
Array_value::to_string() const
{
  std::string str(len, '\0');
  std::transform(data, data + len, str.begin(), [](Value const& v) -> char {
    return v.get_integer();
  });
  return str;
}



std::ostream&
operator<<(std::ostream& os, Value const& v)
{
  switch (v.kind()) {
    case error_value:
      return os << "<error>";

    case integer_value:
      return os << v.get_integer();

    case function_value:
      return os << v.get_function()->name()->spelling();

    case reference_value:
      return os << *v.get_reference() << '@' << (void*)v.get_reference();

    case array_value: {
      os << '{';
      Value const* p = v.get_array().data;
      Value const* q = p + v.get_array().len;
      while (p != q) {
        os << *p;
        if (p + 1 != q)
          os << ',';
        ++p;
      }
      os << '}';
      return os;
    }
  }
  throw std::runtime_error("unhandled value");
}
