// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SCOPE_HPP
#define BEAKER_SCOPE_HPP

// The specialized environment for lexical scoping.

#include <beaker/environment.hpp>
#include <beaker/overload.hpp>


// A scope defines a maximal lexical region of a program
// where no bindings are destroyed. A scope optionally
// assocaites a declaration with its bindings. This is
// used to maintain the current declaration context.
struct Scope : Environment<Symbol const*, Overload>
{
  Scope()
    : decl(nullptr)
  { }

  Scope(Decl* d)
    : decl(d)
  { }

  Decl* decl;
};


// The scope stack maintains the current scope during
// elaboration. It adapts the more general stack to
// provide more language-specific names for those
// operations.
struct Scope_stack : Stack<Scope>
{
  Scope&       current()       { return top(); }
  Scope const& current() const { return top(); }

  Scope&       global()       { return bottom(); }
  Scope const& global() const { return bottom(); }

  Decl*          context() const;
  Module_decl*   module() const;
  Function_decl* function() const;
  Record_decl*   record() const;
};


#endif
