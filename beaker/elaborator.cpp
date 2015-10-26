// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "elaborator.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "stmt.hpp"

#include <iostream>


// -------------------------------------------------------------------------- //
// Lexical scoping


// Create a declarative binding for d. This also checks
// that the we are not redefining a symbol in the current 
// scope.
void
Scope_stack::declare(Decl* d)
{
  Scope& scope = current();

  // TODO: If we allow overloading, then this is
  // where we would handle that.
  //
  // FIXME: Actually diagnose the error. Also, we
  // probably don't need to throw an exception, but
  // simply indicate that the failure.
  if (scope.lookup(d->name()))
    throw std::runtime_error("definition error");
  
  // Create the binding.
  scope.bind(d->name(), d);

  // Set d's declaration context.
  d->cxt_ = context();
}


// Returns the innermost declaration context.
Decl*
Scope_stack::context() const
{
  for (auto iter = rbegin(); iter != rend(); ++iter) {
    Scope const& s = *iter;
    if (s.decl)
      return s.decl;
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
    Scope const& s = *iter;
    if (Function_decl* fn = as<Function_decl>(s.decl))
      return fn;
  }
  return nullptr;
}



// -------------------------------------------------------------------------- //
// Type checking

// FIXME: Improve diagnostics.

template<typename T>
Type const*
check_unary_arithmetic_expr(Elaborator& elab, T const* e)
{
  Type const* z = get_integer_type();
  Type const* t = elab.elaborate(e->first);
  if (t != z)
    throw std::runtime_error("type error");
  return z;
}


template<typename T>
Type const*
check_binary_arithmetic_expr(Elaborator& elab, T const* e)
{
  Type const* z = get_integer_type();
  Type const* t1 = elab.elaborate(e->first);
  if (t1 != z)
    throw std::runtime_error("type error");
  Type const* t2 = elab.elaborate(e->second);
  if (t2 != z)
    throw std::runtime_error("type error");
  return z;
}


template<typename T>
Type const*
check_ordering_expr(Elaborator& elab, T const* e)
{
  Type const* z = get_integer_type();
  Type const* b = get_boolean_type();
  Type const* t1 = elab.elaborate(e->first);
  if (t1 != z)
    throw std::runtime_error("type error");
  Type const* t2 = elab.elaborate(e->second);
  if (t2 != z)
    throw std::runtime_error("type error");
  return b;
}


// In types of the operands shall be the same.
template<typename T>
Type const*
check_equality_expr(Elaborator& elab, T const* e)
{
  Type const* t1 = elab.elaborate(e->first);
  Type const* t2 = elab.elaborate(e->second);
  if (t1 != t2)
    throw std::runtime_error("type error");
  return get_boolean_type();
}


template<typename T>
Type const*
check_unary_logical_expr(Elaborator& elab, T const* e)
{
  Type const* b = get_boolean_type();
  Type const* t = elab.elaborate(e->first);
  if (t != b)
    throw std::runtime_error("type error");
  return b;
}


template<typename T>
Type const*
check_binary_logical_expr(Elaborator& elab, T const* e)
{
  Type const* b = get_boolean_type();
  Type const* t1 = elab.elaborate(e->first);
  if (t1 != b)
    throw std::runtime_error("type error");
  Type const* t2 = elab.elaborate(e->second);
  if (t2 != b)
    throw std::runtime_error("type error");
  return b;
}


// -------------------------------------------------------------------------- //
// Elaboration of expressions

// Returns the type of an expression. This also annotates
// the expression by saving the computed type as part of
// the expression.
Type const* 
Elaborator::elaborate(Expr* e)
{
  struct Fn
  {
    Elaborator& elab;

    Type const* operator()(Literal_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Id_expr* e) const { return elab.elaborate(e); }

    Type const* operator()(Add_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Sub_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Mul_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Div_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Rem_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Neg_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Pos_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Eq_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Ne_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Lt_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Gt_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Le_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Ge_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(And_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Or_expr* e) const { return elab.elaborate(e); }
    Type const* operator()(Not_expr* e) const { return elab.elaborate(e); }

    Type const* operator()(Call_expr* e) const { return elab.elaborate(e); }
  };

  // Check to see if we've already computed the type.
  if (!e->type())
    e->type(apply(e, Fn{*this}));
  return e->type();
}


Type const*
Elaborator::elaborate(Literal_expr* e)
{
  if (is<Boolean_sym>(e->symbol()))
    return get_boolean_type();
  else if (is<Integer_sym>(e->symbol()))
    return get_integer_type();
  else
    throw std::runtime_error("untyped literal");
}


// Elaborate an id expression.
//
// TODO: There may be some contexts in which an unresolved
// identifier can be useful. Unfortunately, this means that
// we have to push the handling of lookup errors up one
// layer, unless we to precisely establish contexts where
// such identifiers are allowed.
Type const*
Elaborator::elaborate(Id_expr* e)
{
  Scope::Binding const* b = stack.lookup(e->symbol());
  if (!b)
    throw std::runtime_error("lookup error");
  return b->second->type();
}


Type const* 
Elaborator::elaborate(Add_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Sub_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Mul_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Div_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Rem_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Neg_expr* e)
{
  return check_unary_arithmetic_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Pos_expr* e)
{
  return check_unary_arithmetic_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Eq_expr* e)
{
  return check_equality_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Ne_expr* e)
{
  return check_equality_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Lt_expr* e)
{
  return check_ordering_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Gt_expr* e)
{
  return check_ordering_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Le_expr* e)
{
  return check_ordering_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Ge_expr* e)
{
  return check_ordering_expr(*this, e);
}


Type const* 
Elaborator::elaborate(And_expr* e)
{
  return check_binary_logical_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Or_expr* e)
{
  return check_binary_logical_expr(*this, e);
}


Type const* 
Elaborator::elaborate(Not_expr* e)
{
  return check_unary_logical_expr(*this, e);
}


Type const*
Elaborator::elaborate(Call_expr* e)
{
  Type const* t1 = elaborate(e->target());
  if (!is<Function_type>(t1))
    throw std::runtime_error("call to non-function");
  Function_type const* t = cast<Function_type>(t1);
  
  // FIXME: Match arguments against parameter types.
  Type_seq const& parms = t->parameter_types();
  Expr_seq const& args = e->arguments();
  if (args.size() < parms.size())
    throw std::runtime_error("too few arguments");
  if (parms.size() < args.size())
    throw std::runtime_error("too many arguments");

  for (std::size_t i = 0; i < parms.size(); ++i) {
    Type const* p = parms[i];

    // TODO: Allow conversions from e to p.
    Expr* e = args[i];
    Type const* a = elaborate(e);
    if (p != a) {
      std::cerr << "type mismatch in argument " << i + 1 << '\n';
      std::cerr << "  expected " << p << " but got " << a << '\n';
      throw std::runtime_error("type mismatch");
    }
  }

  return t->return_type();
}


// -------------------------------------------------------------------------- //
// Elaboration of declarations

// Elaborate a declaration. This returns true if
// elaboration succeeds and false otherwise.
Type const*
Elaborator::elaborate(Decl* d)
{
  struct Fn
  {
    Elaborator& elab;

    Type const* operator()(Variable_decl* d) const { return elab.elaborate(d); }
    Type const* operator()(Function_decl* d) const { return elab.elaborate(d); }
    Type const* operator()(Parameter_decl* d) const { return elab.elaborate(d); }
    Type const* operator()(Module_decl* d) const { return elab.elaborate(d); }
  };

  return apply(d, Fn{*this});
}


// The type of the initializer shall match the declared type
// of the variable.
//
// The variable is declared prior to the elaboration of its
// initializer.
Type const*
Elaborator::elaborate(Variable_decl* d)
{
  stack.declare(d);

  Type const* t = elaborate(d->init());
  if (t != d->type())
    throw std::runtime_error("initializer type error");
  return d->type();
}


// The types of return expressions shall match the declared
// return type of the function.
Type const*
Elaborator::elaborate(Function_decl* d)
{
  stack.declare(d);

  // Remember if we've seen a function named main().
  //
  // FIXME: Compare symbols, not strings.
  if (d->name()->spelling() == "main")
    main = d;

  // Enter the function scope and declare all
  // of the parameters (by way of elaboration).
  //
  // TODO: Handle failed parameter elaborations.
  Scope_sentinel scope(*this, d);
  for (Decl* p : d->parameters())
    elaborate(p);

  // Check the body of the function.
  if (!elaborate(d->body()))
    return nullptr;

  // TODO: Build a control flow graph and ensure that
  // every branch returns a value.

  return d->type();
}


// Elaborate a parameter declaration. This simply declares
// the parameter in the current scope.
Type const*
Elaborator::elaborate(Parameter_decl* d)
{
  stack.declare(d);
  return d->type();
}


// Elaborate the module.  Returns true if successful and
// false otherwise.
Type const*
Elaborator::elaborate(Module_decl* m)
{
  Scope_sentinel scope(*this, m);
  for (Decl* d : m->declarations())
    elaborate(d);
  
  // FIXME: Return a module type.
  return get_boolean_type();
}


// -------------------------------------------------------------------------- //
// Elaboration of statements

// Elaborate a statement. This returns true if elaboration
// succeeds and false otherwise.
Type const*
Elaborator::elaborate(Stmt* s)
{
  struct Fn
  {
    Elaborator& elab;

    Type const* operator()(Empty_stmt* d) const { return elab.elaborate(d); }
    Type const* operator()(Block_stmt* d) const { return elab.elaborate(d); }
    Type const* operator()(Return_stmt* d) const { return elab.elaborate(d); }
    Type const* operator()(Expression_stmt* d) const { return elab.elaborate(d); }
    Type const* operator()(Declaration_stmt* d) const { return elab.elaborate(d); }
  };

  return apply(s, Fn{*this});
}


// FIXME: All of these functions should return the void type.


Type const*
Elaborator::elaborate(Empty_stmt*)
{
  return get_boolean_type();
}


Type const*
Elaborator::elaborate(Block_stmt* s)
{
  Scope_sentinel scope = *this;
  for (Stmt* s1 : s->statements())
    elaborate(s1);
  return get_boolean_type();
}


// The type of the returned expression shall match the declared
// return type of the enclosing function.
//
// TODO: Implement me.
Type const*
Elaborator::elaborate(Return_stmt* s)
{
  Function_decl* fn = stack.function();

  // Check that the return type matches the returned value.
  //
  // TODO: Provide better diagnostics for return values in
  // void functions (after adding a void type).
  Type const* t = elaborate(s->value());
  if (t != fn->return_type())
    throw std::runtime_error("return type mismatch");

  return get_boolean_type();
}


Type const*
Elaborator::elaborate(Expression_stmt* s)
{
  return elaborate(s->expression());
}


Type const*
Elaborator::elaborate(Declaration_stmt* s)
{
  return elaborate(s->declaration());
}
