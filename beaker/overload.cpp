// Copyright (c) 2015 Flowgrammable.org
// All rights reserved

#include "overload.hpp"
#include "type.hpp"
#include "decl.hpp"

#include <iostream>

Symbol const*
Overload::name() const
{
  return front()->name();
}


namespace
{

// Emit a diagnostic explaining why d1 cannot be 
// overloaded with d2. This always returns false.
bool
diagnose_error(Decl const* d1, Decl const* d2, std::string const& reason)
{
  std::cerr << "Cannot overload " << *d1->name() << " of type " << d1->type()
            << " with " << *d2->name() << " of type " << d2->type() << '\n';
  throw std::runtime_error("Cannot overload");
  return false;
}


// Returns true if `t1` and `t2` vary only in their return type.
//
// TODO: Don't use == and !=? All types are unique values, so this
// may be okay.
bool
vary_only_in_return_type(Function_type const* t1, Function_type const* t2)
{
  return t1->parameter_types() == t2->parameter_types() 
      && t1->return_type() != t2->return_type();
}


// Determine if functions f1 and f2 can be overloaded.
//
//    - Two functions f1 and f2 whose types vary only the in
//      return type cannot be overloaded.
//
// TODO: Extend to include constraints.
bool
can_overload_functions(Function_decl const* f1, Function_decl const* f2)
{
  Function_type const* t1 = f1->type();
  Function_type const* t2 = f2->type();

  if (vary_only_in_return_type(t1, t2))
    return diagnose_error(f1, f2, "functions vary only in their return types");

  return true;
}



// Attempt to insert d into the overload set ovl. Returns
// ovl if successful and nullptr otherwise. Emits an error
// on failure.
//
// The following rules apply to overloaded declarations.
//
//    - Only functions can be overloaded.
//
// Note that d2 is the new declaration.
bool
can_overload(Decl const* d1, Decl const* d2)
{
  // If the types are the same, these declare the same function.
  if (d1->type() == d2->type()) {
    // if d1 has an implementation then it is a redefinition
    if (d1->has_def()) {

      std::cerr << "Redefinition of " << *d1->name() << '\n';
      throw std::runtime_error("redefinition error");
    }
    // otherwise we want to treat d1 as a forward declaration
    // and define it using d2
    // we're still going to return false because they are not to 
    // be overloaded, just defined
    return false;
  }


  if (!::is<Function_decl>(d1))
    return diagnose_error(d1, d2, "declaration of a different kind");
  if (!::is<Function_decl>(d2))
    return diagnose_error(d1, d2, "declaration of a different kind");

  Function_decl const* f1 = ::cast<Function_decl>(d1);
  Function_decl const* f2 = ::cast<Function_decl>(d2);
  return can_overload_functions(f1, f2);
}


} // namespace


// Attempt to overload the given declaration with each of
// the declarations in the overload set. Returns true
// if `decl` can be overloaded and false otherwise, When
// `decl` can be overloaded the declaration is added to `ovl`,
// otherwise no action is takne.
bool
overload_decl(Overload* ovl, Decl* decl)
{
  // Check that ovl will be a distinct overload.
  for (Decl const* d : *ovl) {
    if (!can_overload(d, decl))
      return false;
  }
  ovl->push_back(decl);
  return true;
}


// -------------------------------------------------------------------------- //
//                               Printing

// void 
// print(Printer& p, Overload const* ovl)
// {
//   print("=== overload set for {} ===", ovl->name());
//   for (Decl const* d : *ovl) {
//     print(p, d);
//     print(p, '\n');
//   }
// }


// // FIXME: Make this an sexpr.
// void 
// debug(Printer& p, Overload const* ovl)
// {
//   for (Decl const* d : *ovl) {
//     debug(p, d);
//     debug(p, '\n');
//   }
// }
