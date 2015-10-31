// Copyright (c) 2015 Flowgrammable.org
// All rights reserved

#ifndef BEAKER_OVERLOAD_HPP
#define BEAKER_OVERLOAD_HPP

// The overload module provides facilities for overloading
// declarations.

#include "prelude.hpp"

// An overload set is a set of declarations having the
// same name and scope, but different types or constraints.
//
// An overload set always has at least one member.
struct Overload : std::vector<Decl*> 
{
  using std::vector<Decl*>::vector;

  bool is_singleton() const { return size() == 1; }
  
  Symbol const* name() const;
};


// -------------------------------------------------------------------------- //
//                               Declarations

bool overload_decl(Overload*, Decl*);


// void print(Printer&, Overload const*);
// void debug(Printer&, Overload const*);


#endif

