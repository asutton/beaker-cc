// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "elaborator.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "stmt.hpp"
#include "convert.hpp"
#include "error.hpp"

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
  if (scope.lookup(d->name())) {
    // TODO: Add a note that points to the previous
    // definition.
    std::stringstream ss;
    ss << "redefinition of '" << *d->name() << "'\n";
    throw Lookup_error({}, ss.str());
  }
  
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
// Elaboration of expressions

// Returns the type of an expression. This also annotates
// the expression by saving the computed type as part of
// the expression.
Expr*
Elaborator::elaborate(Expr* e)
{
  struct Fn
  {
    Elaborator& elab;

    Expr* operator()(Literal_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Id_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Add_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Sub_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Mul_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Div_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Rem_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Neg_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Pos_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Eq_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Ne_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Lt_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Gt_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Le_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Ge_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(And_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Or_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Not_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Call_expr* e) const { return elab.elaborate(e); }
    
    // Conversions are created as needed and do not
    // need to be elaborated.
    Expr* operator()(Conversion* e) const { return e; }
  };

  // If the expression has no type, then we need to
  // elaborate it and annotate the object.
  if (!e->type())
    return apply(e, Fn{*this});
  else
    return e;
}


Expr*
Elaborator::elaborate(Literal_expr* e)
{
  if (is<Boolean_sym>(e->symbol()))
    e->type(get_boolean_type());
  else if (is<Integer_sym>(e->symbol()))
    e->type(get_integer_type());
  else
    throw std::runtime_error("untyped literal");
  return e;
}


// Elaborate an id expression. When the identifier refers
// to an object of type T (a variable or parameter), the
// type of the expression is T&. Otherwise, the type of the
// expression is the type of the declaration.
//
// TODO: There may be some contexts in which an unresolved
// identifier can be useful. Unfortunately, this means that
// we have to push the handling of lookup errors up one
// layer, unless we to precisely establish contexts where
// such identifiers are allowed.
Expr*
Elaborator::elaborate(Id_expr* e)
{
  Scope::Binding const* b = stack.lookup(e->symbol());
  if (!b) {
    std::stringstream ss;
    ss << "no matching declaration for '" << *e->symbol() << '\'';
    throw Lookup_error(locs.get(e), ss.str());
  }

  // Annotate the expression with its declaration.
  Decl* d = b->second;
  e->declaration(d);

  // If the referenced declaration is a variable of
  // type T, then the type is T&. Otherwise, it is just T.
  Type const* t = d->type();
  if (defines_object(d))
    t = t->ref();
  e->type(t);

  return e;
}


namespace
{

// Elaborate the operands of a binary expression
// whose operands are required to be rvalues. This
// applies implicit convesions as needed.
void
convert_rvalue_operands(Elaborator& elab, Binary_expr* e, Type const* t)
{
  // Process sub-expressions.
  elab.elaborate(e->left());
  elab.elaborate(e->right());

  // Apply conversions, updating the expression.
  e->first = convert(e->left(), t);
  e->second = convert(e->right(), t);
}


// Elaborate the operand of unary expression whose
// operand is required to be an rvalue. This applies
// implicit conversions as needed.
void
convert_rvalue_operands(Elaborator& elab, Unary_expr* e, Type const* t)
{
  // Process the operand.
  elab.elaborate(e->operand());

  // Apply conversions.
  e->first = convert(e->operand(), t);
}


// The operands of a binary arithmetic expression are 
// converted to rvalues. The converted operands shall have 
// type int. The result of an arithmetic expression is an 
// rvalue with type int.
Expr*
check_binary_arithmetic_expr(Elaborator& elab, Binary_expr* e)
{
  Type const* z = get_integer_type();
  convert_rvalue_operands(elab, e, z);
  
  Type const* t1 = e->left()->type();
  Type const* t2 = e->right()->type();
  if (t1 != get_integer_type())
    throw Type_error({}, "left operand does not have type 'int'");
  if (t2 != get_integer_type())
    throw Type_error({}, "right operand does not have type 'int'");
  e->type(z);

  return e;
}


// The operands of a unary arithmetic expression are 
// converted to rvalues. The converted operands shall 
// have type int. The result of an arithmetic expression 
// is an rvalue of type int.
Expr*
check_unary_arithmetic_expr(Elaborator& elab, Unary_expr* e)
{
  Type const* z = get_integer_type();
  convert_rvalue_operands(elab, e, z);

  Type const* t = e->operand()->type();
  if (t != z)
    throw Type_error({}, "operand does not have type 'int'");
  e->type(z);

  return e;
}


} // namespace


Expr*
Elaborator::elaborate(Add_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Sub_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Mul_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Div_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Rem_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


// 
Expr*
Elaborator::elaborate(Neg_expr* e)
{
  return check_unary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Pos_expr* e)
{
  return check_unary_arithmetic_expr(*this, e);
}


namespace
{

// The operands of an equality expression are converted
// to rvalues. The operands shall have the same type. The
// result of an equality expression is an rvalue of type
// bool.
Expr*
check_equality_expr(Elaborator& elab, Binary_expr* e)
{
  elab.elaborate(e->left());
  elab.elaborate(e->right());

  // Apply conversions and update the expression.
  e->first = convert(e->left(), e->left()->type()->nonref());
  e->second = convert(e->right(), e->right()->type()->nonref());

  Type const* t1 = e->left()->type();
  Type const* t2 = e->right()->type();
  if (t1 != t2)
    throw Type_error({}, "operands have different types");
  e->type(get_boolean_type());
  
  return e;
}

} // naespace


Expr*
Elaborator::elaborate(Eq_expr* e)
{
  return check_equality_expr(*this, e);
}


Expr*
Elaborator::elaborate(Ne_expr* e)
{
  return check_equality_expr(*this, e);
}



namespace
{

// The operands of an ordering expression are converted
// to rvalues. The operands shall have type int. The
// result of an equality expression is an rvalue of type
// bool.
Expr*
check_ordering_expr(Elaborator& elab, Binary_expr* e)
{
  Type const* z = get_integer_type();
  convert_rvalue_operands(elab, e, z);
  
  Type const* t1 = e->left()->type();
  Type const* t2 = e->right()->type();
  if (t1 != z)
    throw Type_error({}, "left operand does not have type 'int'");
  if (t2 != z)
    throw Type_error({}, "left operand does not have type 'int'");
  e->type(get_boolean_type());
  return e;
}


} // naespace


Expr*
Elaborator::elaborate(Lt_expr* e)
{
  return check_ordering_expr(*this, e);
}


Expr*
Elaborator::elaborate(Gt_expr* e)
{
  return check_ordering_expr(*this, e);
}


Expr*
Elaborator::elaborate(Le_expr* e)
{
  return check_ordering_expr(*this, e);
}


Expr*
Elaborator::elaborate(Ge_expr* e)
{
  return check_ordering_expr(*this, e);
}


namespace
{


// TODO: Document me!
Expr*
check_binary_logical_expr(Elaborator& elab, Binary_expr* e)
{
  Type const* b = get_boolean_type();
  convert_rvalue_operands(elab, e, b);

  Type const* t1 = e->left()->type();
  Type const* t2 = e->right()->type();
  if (t1 != b)
    throw Type_error({}, "left operand does not have type 'bool'");
  if (t2 != b)
    throw Type_error({}, "right operand does not have type 'bool'");
  e->type(b);

  return e;
}


// TODO: Document me!
Expr*
check_unary_logical_expr(Elaborator& elab, Unary_expr* e)
{
  Type const* b = get_boolean_type();
  convert_rvalue_operands(elab, e, b);

  Type const* t = e->operand()->type();
  if (t != b)
    throw Type_error({}, "operand does not have type 'bool'");
  e->type(b);
  
  return e;
}

} // namespace


Expr*
Elaborator::elaborate(And_expr* e)
{
  return check_binary_logical_expr(*this, e);
}


Expr*
Elaborator::elaborate(Or_expr* e)
{
  return check_binary_logical_expr(*this, e);
}


Expr* 
Elaborator::elaborate(Not_expr* e)
{
  return check_unary_logical_expr(*this, e);
}


// The target function operand is converted to
// an rvalue and shall have funtion type.
Expr*
Elaborator::elaborate(Call_expr* e)
{
  // Apply lvalue to rvalue conversion and ensure that
  // the target has function type.
  elaborate(e->target());
  e->first = lvalue_to_rvalue(e->target(), e->target()->type()->nonref());
  Type const* t1 = e->target()->type();
  if (!is<Function_type>(t1))
    throw Type_error({}, "call to non-function");
  Function_type const* t = cast<Function_type>(t1);
  
  // Check for basic function arity.
  Type_seq const& parms = t->parameter_types();
  Expr_seq& args = e->arguments();
  if (args.size() < parms.size())
    throw Type_error({}, "too few arguments");
  if (parms.size() < args.size())
    throw Type_error({}, "too many arguments");

  // Check that each argument conforms to the the
  // parameter. 
  for (std::size_t i = 0; i < parms.size(); ++i) {
    Type const* p = parms[i];
    Expr*& a = args[i];

    // TODO: Allow conversions from e to p.
    elaborate(a);

    // Apply conversions and update the original
    // expression.
    //
    // TODO: If we fail to find a conversion, then a
    // type mismatch is guaranteed. The check below
    // would be redundant.
    a = convert(a, p);

    // Check for type match.
    if (a->type() != p) {
      std::stringstream ss;
      ss << "type mismatch in argument " << i + 1 << '\n';
      throw Type_error({}, ss.str());
    }
  }

  // The type of the expression is that of the
  // function return type.
  e->type(t->return_type());

  return e;
}


// -------------------------------------------------------------------------- //
// Elaboration of declarations

// Elaborate a declaration. This returns true if
// elaboration succeeds and false otherwise.
void
Elaborator::elaborate(Decl* d)
{
  struct Fn
  {
    Elaborator& elab;

    void operator()(Variable_decl* d) const { return elab.elaborate(d); }
    void operator()(Function_decl* d) const { return elab.elaborate(d); }
    void operator()(Parameter_decl* d) const { return elab.elaborate(d); }
    void operator()(Module_decl* d) const { return elab.elaborate(d); }
  };

  return apply(d, Fn{*this});
}


// The type of the initializer shall match the declared type
// of the variable.
//
// The variable is declared prior to the elaboration of its
// initializer.
void
Elaborator::elaborate(Variable_decl* d)
{
  stack.declare(d);

  // Elaborate the initializer.
  elaborate(d->init());

  // Try converting the initializer to the declared
  // type of the variable. Note that errors may occur
  // during conversion.
  Expr* c = convert(d->init(), d->type());
  if (c->type() != d->type())
    throw Type_error({}, "type mismatch in initializer");
}


// The types of return expressions shall match the declared
// return type of the function.
void
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
  elaborate(d->body());

  // TODO: Build a control flow graph and ensure that
  // every branch returns a value.
}


// Elaborate a parameter declaration. This simply declares
// the parameter in the current scope.
void
Elaborator::elaborate(Parameter_decl* d)
{
  stack.declare(d);
}


// Elaborate the module.  Returns true if successful and
// false otherwise.
void
Elaborator::elaborate(Module_decl* m)
{
  Scope_sentinel scope(*this, m);
  for (Decl* d : m->declarations())
    elaborate(d);
}


// -------------------------------------------------------------------------- //
// Elaboration of statements

// Elaborate a statement. This returns true if elaboration
// succeeds and false otherwise.
void
Elaborator::elaborate(Stmt* s)
{
  struct Fn
  {
    Elaborator& elab;

    void operator()(Empty_stmt* d) const { elab.elaborate(d); }
    void operator()(Block_stmt* d) const { elab.elaborate(d); }
    void operator()(Assign_stmt* d) const { elab.elaborate(d); }
    void operator()(Return_stmt* d) const { elab.elaborate(d); }
    void operator()(If_then_stmt* d) const { elab.elaborate(d); }
    void operator()(If_else_stmt* d) const { elab.elaborate(d); }
    void operator()(While_stmt* d) const { elab.elaborate(d); }
    void operator()(Break_stmt* d) const { elab.elaborate(d); }
    void operator()(Continue_stmt* d) const { elab.elaborate(d); }
    void operator()(Expression_stmt* d) const { elab.elaborate(d); }
    void operator()(Declaration_stmt* d) const { elab.elaborate(d); }
  };

  return apply(s, Fn{*this});
}


void
Elaborator::elaborate(Empty_stmt*)
{
}


void
Elaborator::elaborate(Block_stmt* s)
{
  Scope_sentinel scope = *this;
  for (Stmt* s1 : s->statements())
    elaborate(s1);
}


// In an assignment expression, the left operand shall
// refer to a mutable object. The types of the left and
// right operands shall match.
//
// TODO: The current interpretation of "refers to a mutable
// object" is "is an id-expression".
//
// TODO: If we have const types, then we'd have to add this
// checking.
void
Elaborator::elaborate(Assign_stmt* s)
{
  elaborate(s->object());
  if (!is_lvalue(s->object()))
    throw Type_error({}, "assignment to rvalue");

  // Apply rvalue conversion to the value and update the
  // expression.
  elaborate(s->value());
  s->second = lvalue_to_rvalue(s->value());

  // The types shall match. Compare t1 using the non-reference
  // type of the object.
  Type const* t1 = s->object()->type()->nonref();
  Type const* t2 = s->value()->type();
  if (t1 != t2)
    throw Type_error({}, "assignment to an object of a different type");
}


// The type of the returned expression shall match the declared
// return type of the enclosing function.
//
// TODO: Implement me.
void
Elaborator::elaborate(Return_stmt* s)
{
  Function_decl* fn = stack.function();
  Type const* t = fn->return_type();

  // Check that the return type matches the returned value.
  elaborate(s->value());
  Expr* c = convert(s->value(), t);
  if (c->type() != t)
    throw std::runtime_error("return type mismatch");
}


// The condition must must be a boolean expression.
void
Elaborator::elaborate(If_then_stmt* s)
{
  Type const* b = get_boolean_type();
  
  // Apply rvalue conversions.
  elaborate(s->condition());
  s->first = convert(s->condition(), b);
  if (s->condition()->type() != b)
    throw Type_error({}, "if condition does not have type 'bool'");

  elaborate(s->body());
}


// The condition must must be a boolean expression.
void
Elaborator::elaborate(If_else_stmt* s)
{
  Type const* b = get_boolean_type();
  
  elaborate(s->condition());
  if (s->condition()->type() != b)
  s->first = convert(s->condition(), b);
    throw Type_error({}, "if condition does not have type 'bool'");

  elaborate(s->true_branch());
  elaborate(s->false_branch());
}


void
Elaborator::elaborate(While_stmt* s)
{
  Type const* b = get_boolean_type();
  
  elaborate(s->condition());
  s->first = convert(s->condition(), b);
  if (s->condition()->type() != b)
    throw Type_error({}, "loop condition does not have type 'bool'");

  elaborate(s->body());
}


void
Elaborator::elaborate(Break_stmt* s)
{
  // TODO: Verify that a break occurs within an
  // appropriate context.
}


void
Elaborator::elaborate(Continue_stmt* s)
{
  // TODO: Verify that a continue occurs within an 
  // appropriate context.
}


void
Elaborator::elaborate(Expression_stmt* s)
{
  elaborate(s->expression());
}


void
Elaborator::elaborate(Declaration_stmt* s)
{
  elaborate(s->declaration());
}
