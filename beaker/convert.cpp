// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/convert.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"

#include <iostream>


// If e has reference type T&, return a conversion
// to the value type T. Otherwise, no conversions
// are required and e is returned.
Expr*
convert_to_value(Expr* e)
{
  if (Reference_type const* t = as<Reference_type>(e->type()))
    return new Value_conv(t->nonref(), e);
  else
    return e;
}


// If e has type T[N], return a conversion to
// the chunk T[]. Otherwise, just return e.
Expr*
convert_to_block(Expr* e)
{
  if (Array_type const* a = as<Array_type>(e->type()))
    return new Block_conv(get_block_type(a->type()), e);
  else
    return e;
}

Expr*
convert_to_derived(Expr* e){
  if (Record_type const* r = as<Record_type>(e->type()->nonref())) {
    // std::cout << "here " << *e->type() << std::endl;
    return new Derived_conv(get_record_type(r->declaration()), e);
  }
   else
    return e;
}
// Find a conversion from e to t. If no such
// conversion exists, return nullptr. Diagnostics
// are better handled in the calling context.
Expr*
convert(Expr* e, Type const* t)
{
  // If e has type t, no conversions are needed.
  if (e->type() == t)
    return e;
  Expr* c = e;

  // Ojbect/value transformations

  // If t is a non-reference type, try an 
  // object-to-value conversion:
  //
  //    A& -> B
  if (!is<Reference_type>(t)) {
    c = convert_to_value(e);
    if (c->type() == t)
      return c;
  }//bit cast

  // Type conversions

  // Determine if we can apply an array-to-block
  // conversion. This is the case when we have
  //
  //    A[N] -> B[]
  // std::cout << *t << "\n";
  // std::cout << *c << "\n"; // prints out d

  if (is<Block_type>(t)) {
    c = convert_to_block(c);
    if (c->type() == t)
      return c;
  } else if (is<Reference_type>(t)) {
      const Reference_type* v = cast<Reference_type>(t);
      if(Record_type const* goal = as<Record_type>(v->type())) {
        if (is_derived(c->type()->nonref(), v->type())) {
          Derived_conv *ret = as<Derived_conv>(convert_to_derived(c));
          Record_type const *d = as<Record_type>(c->type()->nonref());
          // Build path from goal to derived
          if(goal->declaration() == d->declaration()){
            return ret;
          }else{
            ret->path_.push_back(0);
            Record_decl* decl = d->declaration();
            while(decl && decl != goal->declaration()){
              ret->path_.push_back(0);
              decl = decl->base()->declaration();
            }
            return ret;
          }
        }

      }

      //if (c->type() == t)
        return c;
  }

  // If we've exhaused all possible conversions
  // without matching the type, then just return
  // nullptr.
  // adjustment
  //conversion
  //qualification

  return nullptr;
}


// Convert a sequence of arguments to a corresponding
// parameter type. The conversion is successful only
// when all individual conversions are successful.
// This is the case when the result vector contains
// no null pointers.
Expr_seq
convert(Expr_seq const& args, Type_seq const& parms)
{
  Expr_seq conv(args.size(), nullptr);

  // If there is a mismatch in size, just return.
  //
  // TODO: Handle variadic funcitons and default
  // arguments.
  if (args.size() < parms.size())
    return conv;
  if (parms.size() < args.size())
    return conv;

  // Convert each argument in turn.
  //
  // TODO: Note that we actually perform initialization
  // for each argument. How does that interoperate with
  // conversions?
  for (std::size_t i = 0; i < parms.size(); ++i)
    conv[i] = convert(args[i], parms[i]);

  return conv;
}
