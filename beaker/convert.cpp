
#include "convert.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"

#include <iostream>


// An id-expression is an object expression when
// it refers to a variable declaration.
//
// TODO: Is an id-expression that names a function
// an object expression? C++ doesn't give it a value
// category.
bool
is_lvalue(Id_expr const* e)
{
  return is<Variable_decl>(e->declaration());
}


// Returns true if e is an lvalue expression.
//
// FIXME: Can't we just look at the type of the
// expression? If it's a T&, then its an lvalue.
// Otherwise, it's an rvalue, right?
bool 
is_lvalue(Expr const* e)
{
  struct Fn
  {
    bool operator()(Literal_expr const* e) { return false; }
    bool operator()(Id_expr const* e) { return is_lvalue(e); }
    bool operator()(Add_expr const* e) { return false; }
    bool operator()(Sub_expr const* e) { return false; }
    bool operator()(Mul_expr const* e) { return false; }
    bool operator()(Div_expr const* e) { return false; }
    bool operator()(Rem_expr const* e) { return false; }
    bool operator()(Neg_expr const* e) { return false; }
    bool operator()(Pos_expr const* e) { return false; }
    bool operator()(Eq_expr const* e) { return false; }
    bool operator()(Ne_expr const* e) { return false; }
    bool operator()(Lt_expr const* e) { return false; }
    bool operator()(Gt_expr const* e) { return false; }
    bool operator()(Le_expr const* e) { return false; }
    bool operator()(Ge_expr const* e) { return false; }
    bool operator()(And_expr const* e) { return false; }
    bool operator()(Or_expr const* e) { return false; }
    bool operator()(Not_expr const* e) { return false; }
    
    // TODO: A call expression is an lvalue expression
    // if the return type is T&.
    bool operator()(Call_expr const* e) { return false; }
    
    bool operator()(Value_conv const* e) { return false; }
  };
  return apply(e, Fn{});
}


// Returns true if e is an rvalue expression.
bool
is_rvalue(Expr* e)
{
  return !is_lvalue(e);
}


// Convert an object expression to a value expression.
Expr*
lvalue_to_rvalue(Expr* e)
{
  assert(e->type());
  if (is_lvalue(e)) {
    Type const* t = e->type()->nonref();
    return new Value_conv(t, e);
  }
  else
    return e;
}


// Apply lvalue-to-rvalue conversions to T 
// only when e has type T&.
Expr*
lvalue_to_rvalue(Expr* e, Type const* t)
{
  if (is<Reference_type>(e->type())) {
    if (e->type()->nonref() == t) {
      return new Value_conv(t, e);
    }
  }
  return e;
}


// Find a conversion from e to t.
//
// Currently, the language supports only 
// lvalue-to-rvalue conversions.
Expr*
convert(Expr* e, Type const* t)
{
  return lvalue_to_rvalue(e, t);
}
