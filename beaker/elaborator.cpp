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


// Returns the current record. The current function is found
// by working outwards through the declaration context stack.
// If there is no current function, this returns nullptr.
Record_decl*
Scope_stack::record() const
{
  for (auto iter = rbegin(); iter != rend(); ++iter) {
    Scope const& s = *iter;
    if (Record_decl* fn = as<Record_decl>(s.decl))
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
    Expr* operator()(Reference_init* e) const { return elab.elaborate(e); }
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
//
// TODO: If the lookup is resolved, should we actually
// return a different kind of expression?
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
  // type T, then the type is T&. Otherwise, it is
  // just T.
  //
  // TODO: Why isn't a function identifier also a
  // reference.
  Type const* t = d->type();
  if (defines_object(d) && !is<Reference_type>(t))
    t = t->ref();
  e->type_ = t;

  return e;
}


namespace
{

// Used to require the conversion of a reference to a
// value. Essentially, this unwraps the reference if
// needed.
Expr*
require_value(Elaborator& elab, Expr* e)
{
  e = elab.elaborate(e);
  e = convert_to_value(e);
  return e;
}


// Used to require the conversion of an expression
// to a given type. This returns nullptr if the convesion
// fails.
Expr*
require_converted(Elaborator& elab, Expr* e, Type const* t)
{
  e = elab.elaborate(e);
  e = convert(e, t);
  return e;
}


// The operands of a binary arithmetic expression are
// converted to rvalues. The converted operands shall have
// type int. The result of an arithmetic expression is an
// rvalue with type int.
template<typename T>
Expr*
check_binary_arithmetic_expr(Elaborator& elab, T* e)
{
  Type const* z = get_integer_type();
  Expr* c1 = require_converted(elab, e->first, z);
  Expr* c2 = require_converted(elab, e->second, z);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'int'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'int'");

  // Rebuild the expression with the
  // converted operands.
  e->type_ = z;
  e->first = c1;
  e->second = c2;
  return e;
}


// The operands of a unary arithmetic expression are
// converted to rvalues. The converted operands shall
// have type int. The result of an arithmetic expression
// is an rvalue of type int.
template<typename T>
Expr*
check_unary_arithmetic_expr(Elaborator& elab, T* e)
{
  // Apply conversions
  Type const* z = get_integer_type();
  Expr* c = require_converted(elab, e->first, z);
  if (!c)
    throw Type_error({}, "operand cannot be converted to 'int'");

  // Rebuild the expression with the converted operands.
  e->type_ = z;
  e->first = c;
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
//
// TODO: Update equality comparison for new types.
Expr*
check_equality_expr(Elaborator& elab, Binary_expr* e)
{
  // Apply conversions.
  Type const* b = get_boolean_type();
  Expr* e1 = require_value(elab, e->first);
  Expr* e2 = require_value(elab, e->second);

  // Check types.
  if (e1->type() != e2->type())
    throw Type_error({}, "operands have different types");

  e->type_ = b;
  e->first = e1;
  e->second = e2;
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
//
// TODO: Update the ordering operands for new types.
Expr*
check_ordering_expr(Elaborator& elab, Binary_expr* e)
{
  // Apply conversions.
  Type const* z = get_integer_type();
  Type const* b = get_boolean_type();
  Expr* c1 = require_converted(elab, e->first, z);
  Expr* c2 = require_converted(elab, e->second, z);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'int'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'int'");

  // Rebuild the expression with the converted
  // operands.
  e->type_ = b;
  e->first = c1;
  e->second = c2;
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
  Expr* c1 = require_converted(elab, e->first, b);
  Expr* c2 = require_converted(elab, e->second, b);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'bool'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'bool'");

  // Rebuild the expression with the converted
  // operands.
  e->type_ = b;
  e->first = c1;
  e->second = c2;
  return e;
}


// TODO: Document me!
Expr*
check_unary_logical_expr(Elaborator& elab, Unary_expr* e)
{
  Type const* b = get_boolean_type();
  Expr* c = require_converted(elab, e->first, b);
  if (!c)
    throw Type_error({}, "operand cannot be converted to 'bool'");

  // Rebuild the expression with the converted
  // operand.
  e->type_ = b;
  e->first = c;
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
  Expr* f = require_value(*this, e->target());
  Type const* t1 = f->type();
  if (!is<Function_type>(t1))
    throw Type_error({}, "cannot call to non-function");
  Function_type const* t = cast<Function_type>(t1);

  // If the target is a member expression, then
  // adjust the arguments to supply a first object.
  Expr_seq& args = e->arguments();
  if (Member_expr* m = as<Member_expr>(f))
    args.insert(args.begin(), m->scope());


  // Check for basic function arity.
  Type_seq const& parms = t->parameter_types();
  if (args.size() < parms.size())
    throw Type_error({}, "too few arguments");
  if (parms.size() < args.size())
    throw Type_error({}, "too many arguments");

  // Check that each argument can be converted to each
  // parameter. Rebuild the argument list with the
  // converted arguments.
  //
  // TODO: Note that we actually perform initialization
  // for each argument. How does that interoperate with
  // conversions?
  for (std::size_t i = 0; i < parms.size(); ++i) {
    Type const* p = parms[i];
    Expr* a = require_converted(*this, args[i], p);
    if (!a) {
      std::stringstream ss;
      ss << "type mismatch in argument " << i + 1 << '\n';
      throw Type_error({}, ss.str());
    }
    args[i] = a;
  }

  // The type of the expression is that of the
  // function return type.
  e->type_ = t->return_type();
  e->first = f;
  return e;
}


// TODO: Document the semantics of member access.
//
// TODO: When resolved, this could actually be a
// different kind of expression with a scope and
// a member offset, kind of like an array index.
// We don't need to annotate this expression with
// the position.
Expr*
Elaborator::elaborate(Member_expr* e)
{
  Expr* e1 = elaborate(e->scope());
  if (!is<Reference_type>(e1->type())) {
    std::stringstream ss;
    ss << "cannot access a member of a non-object";
    throw Type_error({}, ss.str());
  }

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
  for (Decl* d1 : d->members())
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
  //
  // FIXME: If e2 refers to a method declaration,
  // then there won't be an offset. Maybe do the
  // general processing in a Dot_expr, and then
  // create different kinds of expressions based
  // on elaboration.
  for (std::size_t i = 0; i < d->fields().size(); ++i) {
    if (e2->declaration() == d->fields()[i]) {
      e->pos_ = i;
      break;
    }
  }
  // assert(e->pos_ != -1);

  // Finally set the type of the expression.
  e->type_ = e2->type();
  e->first = e1;
  e->second = e2;
  return e;
}


// In the expression e1[e2], e1 shall be an object of
// array type T[N] (for some N) or block type T[]. The
// expression e2 shall be an integer value. The result
// type of the expressions is ref T.
//
// Note that e1 is not converted to a value, and in fact
// *must* be a reference to an object. Converting to a
// value will prevent me from creating element pointers
// in code gen, because we need the base pointer from
// which to compute offsets.
Expr*
Elaborator::elaborate(Index_expr* e)
{
  Expr* e1 = elaborate(e->first);
  if (!is<Reference_type>(e1->type())) {
    std::stringstream ss;
    ss << "cannot index into a value";
    throw Type_error({}, ss.str());
  }

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
  Expr* e2 = require_converted(*this, e->second, get_integer_type());

  // The result type shall be ref T.
  e->type_ = get_reference_type(t->type());
  e->first = e1;
  e->second = e2;

  return e;
}


// NOTE: Conversions are created after their source
// expressions  have been elaborated. No action is
// required.

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

  // If copying into a reference, we're actually
  // performing reference initialization. Create
  // a new node and elaborate it.
  if (is<Reference_type>(e->type())) {
    Reference_init* init = new Reference_init(e->type(), e->value());
    return elaborate(init);
  }

  // Otherwise, this actually a copy.
  //
  // TOOD: This should perform a lookup to find a
  // function that implements copies. It could be
  // bitwise copy, a memberwise copy, or a copy
  // constructor.

  // Convert the value to the resulting type.
  Expr* c = require_converted(*this, e->first, e->type_);
  if (!c) {
    std::stringstream ss;
    ss << "type mismatch in copy initializer (expected "
       << *e->type() << " but got " << *e->value()->type() << ')';
    throw Type_error({}, ss.str());
  }
  e->first = c;

  return e;
}


// Note that this is only ever called from the
// elaborator for copy initialization. The type must
// already be elaborated.
Expr*
Elaborator::elaborate(Reference_init* e)
{
  Expr* obj = elaborate(e->object());

  // A reference can only be bound to an object.
  if (!is<Reference_type>(obj->type())) {
    throw Type_error({}, "binding reference to temporary");
  }

  // TODO: Allow t2 to be derived from t1.
  //
  //    struct B { };
  //    struct D : B { };
  //
  //    var obj : D;
  //    var ref : B& = obj;
  //
  // TODO: Allow t2 to be less cv qualified than t1.
  // That would allow bindings to constants:
  //
  //    var T x;
  //    var T const& c = x;
  Type const* t1 = e->type();
  Type const* t2 = obj->type();
  if (t1->nonref() != t2->nonref()) {
    std::stringstream ss;
    ss << "binding reference to an object of a different type"
       << "(expected " << *t1 << " but got " << *t2 << ')';
    throw Type_error({}, ss.str());
  }

  // Update the expression.
  e->first = obj;

  return e;
}


// -------------------------------------------------------------------------- //
// Elaboration of declarations

// Elaborate a declaration. This returns true if
// elaboration succeeds and false otherwise.
Decl*
Elaborator::elaborate(Decl* d)
{
  struct Fn
  {
    Elaborator& elab;

    Decl* operator()(Variable_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Function_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Parameter_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Record_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Field_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Method_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Module_decl* d) const { return elab.elaborate(d); }
  };

  return apply(d, Fn{*this});
}


// The type of the initializer shall match the declared type
// of the variable.
//
// The variable is declared prior to the elaboration of its
// initializer.
Decl*
Elaborator::elaborate(Variable_decl* d)
{
  d->type_ = elaborate(d->type_);

  // Declare the variable.
  stack.declare(d);

  // Elaborate the initializer. Note that the initializers
  // type must be the same as that of the declaration.
  d->init_ = elaborate(d->init());

  // Annotate the initializer with the declared
  // object.
  //
  // TODO: This will probably be an expression in
  // the future.
  cast<Init>(d->init())->decl_ = d;

  return d;
}


// The types of return expressions shall match the declared
// return type of the function.
//
// FIXME: Theres a lot of overlap with the elaboration
// for methods. Merge that code.
Decl*
Elaborator::elaborate(Function_decl* d)
{
  d->type_ = elaborate(d->type_);

  // Declare the function.
  stack.declare(d);

  // Remember if we've seen a function named main().
  //
  // FIXME: This is dumb. We should do this elsewhere
  // in the interpreter.
  if (d->name()->spelling() == "main")
    main = d;

  // Enter the function scope and declare all
  // of the parameters (by way of elaboration).
  //
  // Note that this modifies the origional parameters.
  Scope_sentinel scope(*this, d);
  for (Decl*& p : d->parms_)
    p = elaborate(p);

  // Check the body of the function, if present.
  if (d->body())
    d->body_ = elaborate(d->body());

  // TODO: Are we actually checking returns match
  // the return type?

  // TODO: Build a control flow graph and ensure that
  // every branch returns a value.
  return d;
}


// Elaborate a parameter declaration. This simply declares
// the parameter in the current scope.
Decl*
Elaborator::elaborate(Parameter_decl* d)
{
  d->type_ = elaborate(d->type_);
  stack.declare(d);
  return d;
}


Decl*
Elaborator::elaborate(Record_decl* d)
{
  stack.declare(d);

  Scope_sentinel scope(*this, d);

  // Elaborate fields and then method declarations.
  //
  // TODO: What are the lookup rules for default
  // member initializers. If we do this:
  //
  //    struct S {
  //      x : int = 1;
  //      y : int = x + 2; // Probably ok
  //      a : int = b - 1; // OK?
  //      b : int = 0;
  //      c : int = f();   // OK?
  //      def f() -> int { ... }
  //    }
  //
  // If we allow the 2nd, then we need to do two
  // phase elaboration.
  for (Decl*& f : d->fields_)
    f = elaborate_decl(f);
  for (Decl*& m : d->members_)
    m = elaborate_decl(m);

  // Elaborate member definitions. See comments
  // above about handling member defintions.
  for (Decl*& m : d->members_)
    m = elaborate_def(m);

  return d;
}


Decl*
Elaborator::elaborate(Field_decl* d)
{
  lingo_unreachable();
}


Decl*
Elaborator::elaborate(Method_decl* d)
{
  lingo_unreachable();
}


// Elaborate the module.  Returns true if successful and
// false otherwise.
Decl*
Elaborator::elaborate(Module_decl* m)
{
  Scope_sentinel scope(*this, m);
  for (Decl*& d : m->decls_)
    d = elaborate(d);
  return m;
}


// -------------------------------------------------------------------------- //
// Elaboration of declarations (but not definitions)

namespace
{

// Defined here because of the member template.
struct Elab_decl_fn
{
  Elaborator& elab;

  template<typename T>
  Decl* operator()(T*) const { lingo_unreachable(); }

  // NOTE: Add overloads in order to support nested
  // declarations. Note that supporting nested types
  // would require its full elaboration.
  Decl* operator()(Field_decl* d) const { return elab.elaborate_decl(d); }
  Decl* operator()(Method_decl* d) const { return elab.elaborate_decl(d); }
};


} // namespace

Decl*
Elaborator::elaborate_decl(Decl* d)
{
  return apply(d, Elab_decl_fn{*this});
}


Decl*
Elaborator::elaborate_decl(Field_decl* d)
{
  d->type_ = elaborate(d->type_);
  stack.declare(d);
  return d;
}


Decl*
Elaborator::elaborate_decl(Method_decl* d)
{
  // Generate the type of the implicit this parameter.
  //
  // TODO: Handle constant references.
  Record_decl* rec = stack.record();
  Type const* type = get_reference_type(get_record_type(rec));

  // Re-build the function type.
  Function_type const* ft = cast<Function_type>(elaborate(d->type()));
  Type_seq pt = ft->parameter_types();
  pt.insert(pt.begin(), type);
  Type const* rt = ft->return_type();
  Type const* mt = get_function_type(pt, rt);
  d->type_ = mt;

  // Actually build the implicit this parameter and add it
  // to the front of the list of parameters.
  Symbol const* name = syms.get("this");
  Parameter_decl* self = new Parameter_decl(name, type);
  d->parms_.insert(d->parms_.begin(), self);

  // Note that we don't need to elaborate or declare
  // the funciton parameters because they're only visible
  // within the function body.

  // Now declare the method.
  stack.declare(d);
  return d;
}


// -------------------------------------------------------------------------- //
// Elaboration of definitions

namespace
{

// Defined here because of the member template.
struct Elab_def_fn
{
  Elaborator& elab;

  template<typename T>
  Decl* operator()(T*) const { lingo_unreachable(); }

  Decl* operator()(Field_decl* d) const { return elab.elaborate_def(d); }
  Decl* operator()(Method_decl* d) const { return elab.elaborate_def(d); }
};


} // namespace


Decl*
Elaborator::elaborate_def(Decl* d)
{
  return apply(d, Elab_def_fn{*this});
}


// Nothing to do here now...
Decl*
Elaborator::elaborate_def(Field_decl* d)
{
  return d;
}


Decl*
Elaborator::elaborate_def(Method_decl* d)
{
  // Elaborate and declare parameters.
  Scope_sentinel scope(*this, d);
  for (Decl*& p : d->parms_)
    p = elaborate(p);

  // Check the body of the method. It must be defined.
  d->body_ = elaborate(d->body());

  // TODO: Are we actually checking returns match
  // the return type?

  // TODO: Build a control flow graph and ensure that
  // every branch returns a value.
  return d;
}

// -------------------------------------------------------------------------- //
// Elaboration of statements

// Elaborate a statement. This returns true if elaboration
// succeeds and false otherwise.
Stmt*
Elaborator::elaborate(Stmt* s)
{
  struct Fn
  {
    Elaborator& elab;

    Stmt* operator()(Empty_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Block_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Assign_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Return_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(If_then_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(If_else_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(While_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Break_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Continue_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Expression_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Declaration_stmt* d) const { return elab.elaborate(d); }
  };

  return apply(s, Fn{*this});
}


Stmt*
Elaborator::elaborate(Empty_stmt* s)
{
  return s;
}


Stmt*
Elaborator::elaborate(Block_stmt* s)
{
  Scope_sentinel scope = *this;
  for (Stmt*& s1 : s->first)
    s1 = elaborate(s1);
  return s;
}


// In an assignment expression, the left operand shall
// refer to a mutable object. The types of the left and
// right operands shall match.
//
// TODO: If we have const types, then we'd have to add this
// checking.
Stmt*
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

  s->first = lhs;
  s->second = rhs;
  return s;
}


// The type of the returned expression shall match the declared
// return type of the enclosing function.
//
// TODO: Implement me.
Stmt*
Elaborator::elaborate(Return_stmt* s)
{
  Function_decl* fn = stack.function();
  Type const* t = fn->return_type();

  // Check that the return type matches the returned value.
  Expr* e = elaborate(s->value());
  Expr* c = convert(e, t);
  if (!c) {
    std::stringstream ss;
    ss << "return type mismatch (expected "
       << *t << " but got " << *s->value()->type() << ")";
    throw std::runtime_error(ss.str());
  }

  s->first = c;
  return s;
}


// The condition must must be a boolean expression.
Stmt*
Elaborator::elaborate(If_then_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "if condition does not have type 'bool'");
  Stmt* b = elaborate(s->body());

  s->first = c;
  s->second = b;
  return s;
}


// The condition must must be a boolean expression.
Stmt*
Elaborator::elaborate(If_else_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "if condition does not have type 'bool'");
  Stmt* t = elaborate(s->true_branch());
  Stmt* f = elaborate(s->false_branch());

  s->first = c;
  s->second = t;
  s->third = f;
  return s;
}


Stmt*
Elaborator::elaborate(While_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "loop condition does not have type 'bool'");
  Stmt* b = elaborate(s->body());

  s->first = c;
  s->second = b;
  return s;
}


Stmt*
Elaborator::elaborate(Break_stmt* s)
{
  // TODO: Verify that a break occurs within an
  // appropriate context.
  return s;
}


Stmt*
Elaborator::elaborate(Continue_stmt* s)
{
  // TODO: Verify that a continue occurs within an
  // appropriate context.
  return s;
}


Stmt*
Elaborator::elaborate(Expression_stmt* s)
{
  s->first = elaborate(s->expression());
  return s;
}


Stmt*
Elaborator::elaborate(Declaration_stmt* s)
{
  s->first = elaborate(s->declaration());
  return s;
}
