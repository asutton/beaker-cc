// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/scope.hpp"
#include "beaker/decl.hpp"


// Returns the innermost declaration context.
Decl*
Scope_stack::context() const
{
  for (auto iter = rbegin(); iter != rend(); ++iter) {
    Scope const* s = *iter;
    if (s->decl)
      return s->decl;
  }
  return nullptr;
}


// Returns the current module. This always the bottom
// of the stack.
Module_decl*
Scope_stack::module() const
{
  return cast<Module_decl>(bottom().decl);
}


// Returns the current function. The current function is found
// by working outwards through the declaration context stack.
// If there is no current function, this returns nullptr.
Function_decl*
Scope_stack::function() const
{
  for (auto iter = rbegin(); iter != rend(); ++iter) {
    Scope const* s = *iter;
    if (Function_decl* fn = as<Function_decl>(s->decl))
      return fn;
  }
  return nullptr;
}


// Returns the current record. The current function is found
// by working outwards through the declaration context stack.
// If there is no current function, this returns nullptr.
Record_decl*
Scope_stack::record() const
{
  for (auto iter = rbegin(); iter != rend(); ++iter) {
    Scope const* s = *iter;
    if (Record_decl* fn = as<Record_decl>(s->decl))
      return fn;
  }
  return nullptr;
}

auto
Record_scope::lookup(Symbol const* sym) -> Binding* {
  //TODO: check decl if it's a record?
  //perform record lookup?
  if(Record_decl* fn = as<Record_decl>(this->decl)) {
    if(Binding* bind = Environment::lookup(sym)){
      return bind;
    }
  }
  return nullptr;
}
