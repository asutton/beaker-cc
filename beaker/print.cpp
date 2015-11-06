// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "print.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"

#include <iostream>


std::ostream& 
operator<<(std::ostream& os, Type const* t)
{
  struct Fn
  {
    std::ostream& os;

    void operator()(Record_type const* t) { os << t; }
    void operator()(Boolean_type const* t) { os << t; }
    void operator()(Integer_type const* t) { os << t; }
    void operator()(Function_type const* t) { os << t; }

    // network specific types
    void operator()(Table_type const* t) { os << t; }
    void operator()(Flow_type const* t) { os << t; }
    void operator()(Port_type const* t) { os << t; }
  };

  apply(t, Fn{os});
  return os;
}


std::ostream&
operator<<(std::ostream& os, Boolean_type const*)
{
  return os << "bool";
}


std::ostream&
operator<<(std::ostream& os, Integer_type const*)
{
  return os << "int";
}


std::ostream&
operator<<(std::ostream& os, Function_type const* t)
{
  os << '(';
  Type_seq const& parms = t->parameter_types();
  for (auto iter = parms.begin(); iter != parms.end(); ++iter) {
    os << *iter;
    if (std::next(iter) != parms.end())
      os << ',';
  }
  os << ')';
  os << " -> " << t->return_type();
  return os;
}


std::ostream&
operator<<(std::ostream& os, Record_type const* t)
{
  return os << "record " << t->decl()->name();
}


// FIXME: print key fields correctly
std::ostream& operator<<(std::ostream& os, Table_type const* t) 
{
  // for (auto key : t->key_fields())
  //   os << key;

  return os << "table ";
}


std::ostream& operator<<(std::ostream& os, Flow_type const* t) 
{
  os << "flow(";
  for (auto key : t->key_types())
    os << key << " ";

  os << ")";

  return os;
}

std::ostream& operator<<(std::ostream& os, Port_type const* t) 
{
  return os << "port"; 
}

