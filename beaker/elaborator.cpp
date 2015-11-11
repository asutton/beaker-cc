// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "elaborator.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "stmt.hpp"
#include "convert.hpp"
#include "evaluator.hpp"
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
// Elaboration of types


Type const*
Elaborator::elaborate(Type const* t)
{
  struct Fn
  {
    Elaborator& elab;

    Type const* operator()(Id_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Boolean_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Character_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Integer_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Function_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Block_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Array_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Reference_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Record_type const* t) { return elab.elaborate(t); }
  };
  return apply(t, Fn{*this});
}


Type const*
Elaborator::elaborate(Id_type const* t)
{
  Scope::Binding const* b = stack.lookup(t->symbol());
  if (!b) {
    std::stringstream ss;
    ss << "no matching declaration for '" << *t->symbol() << '\'';
    throw Lookup_error(locs.get(t), ss.str());
  }

  // Determine if the name is a type declaration.
  Decl* d = b->second;
  if (Record_decl* r = as<Record_decl>(d)) {
    return get_record_type(r);
  }
  else {
    std::stringstream ss;
    ss << '\'' << *t->symbol() << "' does not name a type";
    throw Lookup_error(locs.get(t), ss.str());
  }
}


Type const*
Elaborator::elaborate(Boolean_type const* t)
{
  return t;
}


Type const*
Elaborator::elaborate(Character_type const* t)
{
  return t;
}


Type const*
Elaborator::elaborate(Integer_type const* t)
{
  return t;
}


// Elaborate each type in the function type.
Type const*
Elaborator::elaborate(Function_type const* t)
{
  Type_seq ts;
  ts.reserve(t->parameter_types().size());
  for (Type const* t1 : t->parameter_types())
    ts.push_back(elaborate(t1));
  Type const* r = elaborate(t->return_type());
  return get_function_type(ts, r);
}


Type const*
Elaborator::elaborate(Array_type const* t)
{
  Type const* t1 = elaborate(t->type());
  Expr* e = elaborate(t->extent());
  Expr* n = reduce(e);
  if (!n)
    throw Type_error({}, "non-constant array extent");
  return get_array_type(t1, n);
}

Type const*
Elaborator::elaborate(Block_type const* t)
{
  Type const* t1 = elaborate(t->type());
  return get_block_type(t1);
}


Type const*
Elaborator::elaborate(Reference_type const* t)
{
  Type const* t1 = elaborate(t->type());
  return get_reference_type(t1);
}


// No further elaboration is needed.
Type const*
Elaborator::elaborate(Record_type const* t)
{
  return t;
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
    Expr* operator()(Member_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Index_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Value_conv* e) const { return elab.elaborate(e); }
    Expr* operator()(Block_conv* e) const { return elab.elaborate(e); }
    Expr* operator()(Default_init* e) const { return elab.elaborate(e); }
    Expr* operator()(Copy_init* e) const { return elab.elaborate(e); }
  };

  return apply(e, Fn{*this});
}


// Literal expressions are fully elaborated at the point
// of construction.
Expr*
Elaborator::elaborate(Literal_expr* e)
{
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

// Used to require the conversion of a reference to a
// value. Essentially, this unwraps the reference if
// needed.
//
// Note that after completion, the pointer e is modified
// so that it is the same as the return value.
Expr*
require_value(Elaborator& elab, Expr*& e)
{
  e = convert_to_value(elab.elaborate(e));
  return e;
}


// Used to require the conversion of an expression
// to a given type.
//
// Note that after succesful completion, the pointer
// e is modified so that it is the same as the return
// value.
//
// This returns nullptr if the convesion fails.
Expr*
require_converted(Elaborator& elab, Expr*& e, Type const* t)
{
  elab.elaborate(e);

  // Try a conversion. If it succeeds, update
  // the original expression.
  Expr* c = convert(e, t);
  if (c)
    e = c;

  return c;
}


// The operands of a binary arithmetic expression are
// converted to rvalues. The converted operands shall have
// type int. The result of an arithmetic expression is an
// rvalue with type int.
Expr*
check_binary_arithmetic_expr(Elaborator& elab, Binary_expr* e)
{
  Type const* z = get_integer_type();
  Expr* c1 = require_converted(elab, e->first, z);
  Expr* c2 = require_converted(elab, e->second, z);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'int'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'int'");
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
  // Apply conversions
  Type const* z = get_integer_type();
  Expr* c = require_converted(elab, e->first, z);
  if (!c)
    throw Type_error({}, "operand cannot be converted to 'int'");
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
  // Apply conversions.
  Expr* e1 = require_value(elab, e->first);
  Expr* e2 = require_value(elab, e->second);

  // Check types.
  if (e1->type() != e2->type())
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
  // Apply conversions.
  Type const* z = get_integer_type();
  Expr* c1 = require_converted(elab, e->first, z);
  Expr* c2 = require_converted(elab, e->second, z);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'int'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'int'");
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
  // Apply conversions.
  Type const* b = get_boolean_type();
  Expr const* c1 = require_converted(elab, e->first, b);
  Expr const* c2 = require_converted(elab, e->second, b);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'bool'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'bool'");
  e->type(b);
  return e;
}


// TODO: Document me!
Expr*
check_unary_logical_expr(Elaborator& elab, Unary_expr* e)
{
  Type const* b = get_boolean_type();
  Expr const* c = require_converted(elab, e->first, b);
  if (!c)
    throw Type_error({}, "operand cannot be converted to 'bool'");
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
// a value and shall have funtion type.
Expr*
Elaborator::elaborate(Call_expr* e)
{
  // Apply lvalue to rvalue conversion and ensure that
  // the target has function type.
  Expr* f = require_value(*this, e->first);
  Type const* t1 = f->type();
  if (!is<Function_type>(t1))
    throw Type_error({}, "cannot call to non-function");
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
    Expr* a = require_converted(*this, args[i], p);
    if (!a) {
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


// TODO: Document the semantics of member access.
Expr*
Elaborator::elaborate(Member_expr* e)
{
  Expr* e1 = elaborate(e->scope());

  // Get the non-reference type of the outer
  // object so we can perform lookups.
  Record_type const* t = as<Record_type>(e1->type()->nonref());
  if (!t) {
    std::stringstream ss;
    ss << "object does not have record type";
    throw Type_error({}, ss.str());
  }

  // Re-open the class scope so we can perform lookups
  // in the usual way.
  Record_decl* d = t->declaration();
  Scope_sentinel scope(*this, d);
  for (Decl* d1 : d->fields())
    stack.top().bind(d1->name(), d1);

  // Elaborate the member expression.
  Id_expr* e2 = as<Id_expr>(elaborate(e->member()));
  if (!e2) {
    std::stringstream ss;
    ss << "invalid member reference";
    throw Type_error({}, ss.str());
  }

  // Find the offset in the class of the member.
  // And stash it in the member expression.
  for (std::size_t i = 0; i < d->fields().size(); ++i) {
    if (e2->declaration() == d->fields()[i]) {
      e->pos_ = i;
      break;
    }
  }
  assert(e->pos_ != -1);

  // Finally set the type of the expression.
  e->type_ = e2->type();
  return e;
}


// In the expression e1[e2], e1 shall be a value of 
// array type T[N] (for some N) or block type T[]. The
// expression e2 shall be an integer value. The result
// type of the expressions is ref T.
//
// Note that e1 is converted to a value.
Expr*
Elaborator::elaborate(Index_expr* e)
{
  Expr* e1 = require_value(*this, e->first);

  // Get the non-reference type of the array.
  //
  // FIXME: Does this require a value transformation?
  // We don't (yet?) have array literals, so I generally
  // expect that this *must* be a reference to an array.
  //
  // TODO: Allow block type.
  Array_type const* t = as<Array_type>(e1->type()->nonref());
  if (!t) {
    std::stringstream ss;
    ss << "object does not have array type";
    throw Type_error({}, ss.str());
  }

  // The index shall be an integer value.
  require_converted(*this, e->second, get_integer_type());

  // The result type shall be ref T.
  e->type_ = get_reference_type(t->type());

  return e;
}


// NOTE: Conversions are created after their source
// expressions  have been elaborated. No action is
//required.

Expr*
Elaborator::elaborate(Value_conv* e)
{
  return e;
}


Expr*
Elaborator::elaborate(Block_conv* e)
{
  return e;
}


// TODO: I probably need to elaborate the type.
Expr*
Elaborator::elaborate(Default_init* e)
{
  e->type_ = elaborate(e->type_);
  return e;
}


Expr*
Elaborator::elaborate(Copy_init* e)
{
  // Elaborate the type.
  e->type_ = elaborate(e->type_);

  // Convert the value to the resulting type.
  Expr* c = require_converted(*this, e->first, e->type_);
  if (!c) {
    std::stringstream ss;
    ss << "type mismatch in copy initializer (expected "
       << *e->type() << " but got " << *e->value()->type() << ')';
    throw Type_error({}, ss.str());
  }

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
    void operator()(Record_decl* d) const { return elab.elaborate(d); }
    void operator()(Field_decl* d) const { return elab.elaborate(d); }
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
  d->type_ = elaborate(d->type_);

  // Declare the variable.
  stack.declare(d);

  // Elaborate the initializer. Note that the initializers
  // type must be the same as that of the declaration.
  elaborate(d->init());

  // Annotate the initializer with the declared
  // object.
  //
  // TODO: This will probably be an expression in
  // the future.
  cast<Init>(d->init())->decl_ = d;
}


// The types of return expressions shall match the declared
// return type of the function.
void
Elaborator::elaborate(Function_decl* d)
{
  d->type_ = elaborate(d->type_);

  // Declare the function.
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

  // Check the body of the function, if present.
  if (d->body())
    elaborate(d->body());

  // TODO: Build a control flow graph and ensure that
  // every branch returns a value.
}


// Elaborate a parameter declaration. This simply declares
// the parameter in the current scope.
void
Elaborator::elaborate(Parameter_decl* d)
{
  d->type_ = elaborate(d->type_);
  stack.declare(d);
}


void
Elaborator::elaborate(Record_decl* d)
{
  stack.declare(d);
  Scope_sentinel scope(*this, d);
  for (Decl* d1 : d->fields())
    elaborate(d1);
}


void
Elaborator::elaborate(Field_decl* d)
{
  d->type_ = elaborate(d->type_);
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
// TODO: If we have const types, then we'd have to add this
// checking.
void
Elaborator::elaborate(Assign_stmt* s)
{
  // FIXME: Write a better predicate?
  Expr* lhs = elaborate(s->object());
  if (!is<Reference_type>(lhs->type()))
    throw Type_error({}, "assignment to rvalue");

  // Apply rvalue conversion to the value and update the
  // expression.
  Expr *rhs = require_value(*this, s->second);

  // The types shall match. Compare t1 using the non-reference
  // type of the object.
  Type const* t1 = lhs->type()->nonref();
  Type const* t2 = rhs->type();
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
  Expr* c = require_converted(*this, s->first, t);
  if (!c)
    throw std::runtime_error("return type mismatch");
}


// The condition must must be a boolean expression.
void
Elaborator::elaborate(If_then_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "if condition does not have type 'bool'");
  elaborate(s->body());
}


// The condition must must be a boolean expression.
void
Elaborator::elaborate(If_else_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "if condition does not have type 'bool'");
  elaborate(s->true_branch());
  elaborate(s->false_branch());
}


void
Elaborator::elaborate(While_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
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
