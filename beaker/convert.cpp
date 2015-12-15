// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/convert.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"

#include <iostream>

// this determines if a type can be promoted
// promotion to the same type is not valid
bool
can_promote(Expr* e, Type const* t)
{    
    // static types
    static Type const* char_type = get_character_type();
    static Type const* float_type = get_float_type();
    static Type const* double_type = get_double_type();
    
    // check for type match
    if (e->type() == t)
        return false;
    
    // nothing can become a char
    if (t == char_type)
        return false;
    
    // char can become an integer or floating point
    if (e->type() == char_type)
            return true;
    
    // integer types
    if (is_integer(e->type())) {
        // integers can become either floating point type
        if (t == float_type || t == double_type)
            return true;
        
        // must cast as Integer_type* to allow access to precision and sign
        const Integer_type* et = dynamic_cast<const Integer_type*>(e->type());
        const Integer_type* tt = dynamic_cast<const Integer_type*>(t);
        
        // integers can become a greater percision
        if (et->precision() < tt->precision())
            return true;
        
        // integers can move from unsigned to signed
        if (et->is_signed() < tt->is_signed())
            return true;
    }
    
    // float can become a double
    if (e->type() == float_type && t == double_type)
        return true;
    
    // default
    return false;
}

// if promotion is not allowed then the origninal
// expr is returned. This allows for operations 
// with same types to occur.
Expr*
try_promote(Expr* e, Type const* t){
    if (can_promote(e,t))
        return new Promote_conv(t,e);
    else
        return e;
}

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


// Find a conversion from e to t. If no such
// conversion exists, return nullptr. Diagnostics
// a better handled in the calling context.
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
  }

  // Type conversions

  // Determine if we can apply an array-to-chunk
  // conversion. This is the case when we have
  //
  //    A[N] -> B[]
  if (is<Block_type>(t)) {
    c = convert_to_block(c);
    if (c->type() == t)
      return c;
  }
    
  // Try to apply a type promotion
  if (is<Integer_type>(t) || is<Float_type>(t) || is<Double_type>(t)) {
    c = try_promote(e,t);
    if (c->type() == t)
        return c;
  }

  // If we've exhaused all possible conversions
  // without matching the type, then just return
  // nullptr.

  return nullptr;
}


// Convert a seequence of arguments to a corresponding
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
