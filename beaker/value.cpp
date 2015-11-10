// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "value.hpp"
#include "decl.hpp"


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
