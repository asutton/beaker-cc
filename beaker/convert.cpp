
#include "convert.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"

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


// Find a conversion from e to t. If no such
// conversion exists, return nullptr. Diagnostics
// a better handled in the calling context.
Expr*
convert(Expr* e, Type const* t)
{
  std::cout << 1 << std::endl;
  // If e has type t, no conversions are needed.
  if (e->type() == t)
  {
    std::cout << 2 << std::endl;
    return e;
  }
  Expr* c = e;
  std::cout << 3 << std::endl;


  // Ojbect/value transformations

  // If t is a non-reference type, try an
  // object-to-value conversion:
  //
  //    A& -> B
  if (!is<Reference_type>(t)) {
    std::cout << 4 << std::endl;
    c = convert_to_value(e);
    std::cout << 5 << std::endl;
    if (c->type() == t)
    {
      std::cout << 6 << std::endl;
      return c;
    }
  }

  // Type conversions

  // Determine if we can apply an array-to-chunk
  // conversion. This is the case when we have
  //
  //    A[N] -> B[]
  std::cout << 7 << std::endl;
  if (is<Block_type>(t)) {
    std::cout << 8 << std::endl;
    c = convert_to_block(c);
    std::cout << 9 << std::endl;
    if (c->type() == t)
    {
      std::cout << 10 << std::endl;
      return c;
    }
  }
  std::cout << 11 << std::endl;
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
