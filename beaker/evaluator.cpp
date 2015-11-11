// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "evaluator.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "stmt.hpp"
#include "error.hpp"

#include <iostream>


Value
Evaluator::eval(Expr const* e)
{
  struct Fn
  {
    Evaluator& ev;

    Value operator()(Literal_expr const* e) { return ev.eval(e); }
    Value operator()(Id_expr const* e) { return ev.eval(e); }
    Value operator()(Add_expr const* e) { return ev.eval(e); }
    Value operator()(Sub_expr const* e) { return ev.eval(e); }
    Value operator()(Mul_expr const* e) { return ev.eval(e); }
    Value operator()(Div_expr const* e) { return ev.eval(e); }
    Value operator()(Rem_expr const* e) { return ev.eval(e); }
    Value operator()(Neg_expr const* e) { return ev.eval(e); }
    Value operator()(Pos_expr const* e) { return ev.eval(e); }
    Value operator()(Eq_expr const* e) { return ev.eval(e); }
    Value operator()(Ne_expr const* e) { return ev.eval(e); }
    Value operator()(Lt_expr const* e) { return ev.eval(e); }
    Value operator()(Gt_expr const* e) { return ev.eval(e); }
    Value operator()(Le_expr const* e) { return ev.eval(e); }
    Value operator()(Ge_expr const* e) { return ev.eval(e); }
    Value operator()(And_expr const* e) { return ev.eval(e); }
    Value operator()(Or_expr const* e) { return ev.eval(e); }
    Value operator()(Not_expr const* e) { return ev.eval(e); }
    Value operator()(Call_expr const* e) { return ev.eval(e); }
    Value operator()(Member_expr const* e) { return ev.eval(e); }
    Value operator()(Index_expr const* e) { return ev.eval(e); }
    Value operator()(Value_conv const* e) { return ev.eval(e); }
    Value operator()(Block_conv const* e) { return ev.eval(e); }
    Value operator()(Default_init const* e) { return ev.eval(e); }
    Value operator()(Copy_init const* e) { return ev.eval(e); }
  };

  return apply(e, Fn {*this});
}


Value
Evaluator::eval(Literal_expr const* e)
{
  return e->value();
}


Value
Evaluator::eval(Id_expr const* e)
{
  return &stack.lookup(e->symbol())->second;
}


// TODO: Detect overflow.
Value
Evaluator::eval(Add_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return v1.get_integer() + v2.get_integer();
}


// TODO: Detect overflow.
Value
Evaluator::eval(Sub_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return v1.get_integer() - v2.get_integer();
}


// TODO: Detect overflow.
Value
Evaluator::eval(Mul_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return v1.get_integer() * v2.get_integer();
}


Value
Evaluator::eval(Div_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  if (v2.get_integer() == 0)
    throw std::runtime_error("division by 0");
  return v1.get_integer() / v2.get_integer();
}


Value
Evaluator::eval(Rem_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  if (v2.get_integer() == 0)
    throw std::runtime_error("division by 0");
  return v1.get_integer() / v2.get_integer();
}


Value
Evaluator::eval(Neg_expr const* e)
{
  Value v = eval(e->operand());
  return -v.get_integer();
}


Value
Evaluator::eval(Pos_expr const* e)
{
  return eval(e->operand());
}


// Compare two integer or function values.
template<typename F>
bool
compare_equal(Value const& v1, Value const& v2, F fn)
{
  // See through references.
  Value const& a = v1.is_reference() ? *v1.get_reference() : v1;
  Value const& b = v2.is_reference() ? *v2.get_reference() : v2;

  // Perform comparison.
  if (a.kind() == b.kind()) {
    if (a.is_integer())
      return fn(a.get_integer(), b.get_integer());
    if (a.is_function())
      return fn(a.get_function(), b.get_function());
  }
  throw Evaluation_error({}, "invalid operands");
}


Value
Evaluator::eval(Eq_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return compare_equal(v1, v2, std::equal_to<>());
}


// Compare two integer or function values.
Value
Evaluator::eval(Ne_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return compare_equal(v1, v2, std::not_equal_to<>());
}


// Compare two integer or function values.
template<typename F>
bool
compare_less(Value const& v1, Value const& v2, F fn)
{
  Value const& a = v1.is_reference() ? *v1.get_reference() : v1;
  Value const& b = v2.is_reference() ? *v2.get_reference() : v2;

  if (a.kind() == b.kind()) {
    if (a.is_integer())
      return fn(a.get_integer(), b.get_integer());
  }
  throw Evaluation_error({}, "invalid operands");
}

// Order two integer values.
Value
Evaluator::eval(Lt_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return compare_less(v1, v2, std::less<>());
}


Value
Evaluator::eval(Gt_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return compare_less(v1, v2, std::greater<>());
}


Value
Evaluator::eval(Le_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return compare_less(v1, v2, std::less_equal<>());
}


Value
Evaluator::eval(Ge_expr const* e)
{
  Value v1 = eval(e->left());
  Value v2 = eval(e->right());
  return compare_less(v1, v2, std::greater_equal<>());
}


Value
Evaluator::eval(And_expr const* e)
{
  Value v = eval(e->left());
  if (!v.get_integer())
    return v;
  else
    return eval(e->right());
}


Value
Evaluator::eval(Or_expr const* e)
{
  Value v = eval(e->left());
  if (v.get_integer())
    return v;
  else
    return eval(e->right());
}


Value
Evaluator::eval(Not_expr const* e)
{
  Value v = eval(e->operand());
  return !v.get_integer();
}


Value
Evaluator::eval(Call_expr const* e)
{
  // Evaluate the function expression.
  Value v = eval(e->target());
  Function_decl const* f = v.get_function();

  // Evaluate each argument in turn.
  Value_seq args;
  args.reserve(e->arguments().size());
  for (Expr const* a : e->arguments())
    args.push_back(eval(a));

  // Build the new call frame by pushing bindings
  // from each parameter to the corresponding argument.
  //
  // FIXME: Since everything type-checked, these *must*
  // happen to magically line up. However, it would be
  // a good idea to verify.
  Store_sentinel frame(*this);
  for (std::size_t i = 0; i < args.size(); ++i) {
    Decl* p = f->parameters()[i];
    Value& v = args[i];
    stack.top().bind(p->name(), v);
  }

  // Evaluate the function definition.
  //
  // TODO: Check result in case we've thrown
  // an exception (for example).
  Value result;
  Control ctl = eval(f->body(), result);
  if (ctl != return_ctl)
    throw std::runtime_error("function evaluation failed");

  return result;
}


Value
Evaluator::eval(Member_expr const* e)
{
  throw std::runtime_error("not implemented");
}


Value
Evaluator::eval(Index_expr const* e)
{
  throw std::runtime_error("not implemnted");
}


// Apply an object-to-value conversion by dereferencing
// the reference value. Note that the source must evaluate
// to a reference.
Value
Evaluator::eval(Value_conv const* e)
{
  Value v = eval(e->source());
  return *v.get_reference();
}


// Apply an array-to-block conversion by dereferencing
// the reference value. Note that the source must evaluate
// to a reference.
Value
Evaluator::eval(Block_conv const* e)
{
  throw std::runtime_error("not implemented");
}


// FIXME: This is wrong. We should be calling a function
// that default initializes the created object.
Value
Evaluator::eval(Default_init const* e)
{
  if (is_scalar(e->type()))
    return Value(0);
  else
    throw std::runtime_error("unhandled default initializer");
}


// FIXME: This should be calling a function that
// default iniitializes the created object.
Value
Evaluator::eval(Copy_init const* e)
{
  return eval(e->value());
}


// -------------------------------------------------------------------------- //
// Evaluation of declarations

void
Evaluator::eval(Decl const* d)
{
  struct Fn
  {
    Evaluator& ev;

    void operator()(Variable_decl const* d) { ev.eval(d); }
    void operator()(Function_decl const* d) { ev.eval(d); }
    void operator()(Parameter_decl const* d) { ev.eval(d); }
    void operator()(Record_decl const* d) { ev.eval(d); }
    void operator()(Field_decl const* d) { ev.eval(d); }
    void operator()(Module_decl const* d) { ev.eval(d); }
  };

  return apply(d, Fn{*this});
}


void
Evaluator::eval(Variable_decl const* d)
{
  // FIXME: This is wrong. We should create and bind the
  // value first. Then, we should call a constructor
  // passing a reference to the uninitialized object.
  Value v = eval(d->init());

  // Create and bind the value.
  stack.top().bind(d->name(), v).second;
}


// Bind the symbol to a function value.
void
Evaluator::eval(Function_decl const* d)
{
  stack.top().bind(d->name(), d);
}


// This function should never be called.
void
Evaluator::eval(Parameter_decl const*)
{
  return;
}


// There is no evaluation for a record.
void
Evaluator::eval(Record_decl const*)
{
  return;
}


// There is no evaluation for a field.
void
Evaluator::eval(Field_decl const*)
{
  return;
}


// Evaluate the declarations in the module.
void
Evaluator::eval(Module_decl const* d)
{
  Store_sentinel store(*this);
  for (Decl const* d1 : d->declarations())
    eval(d1);
}


// -------------------------------------------------------------------------- //
// Evaluation of statements

// Evaluate the given statement, returning a
// control instruction, which determines how
// the evaluation proceeds. Storage is provided
// for a return value as an output argument.
Control
Evaluator::eval(Stmt const* s, Value& r)
{
  struct Fn
  {
    Evaluator& ev;
    Value& r;

    Control operator()(Empty_stmt const* s) { return ev.eval(s, r); }
    Control operator()(Block_stmt const* s) { return ev.eval(s, r); }
    Control operator()(Assign_stmt const* s) { return ev.eval(s, r); }
    Control operator()(Return_stmt const* s) { return ev.eval(s, r); }
    Control operator()(If_then_stmt const* s) { return ev.eval(s, r); }
    Control operator()(If_else_stmt const* s) { return ev.eval(s, r); }
    Control operator()(While_stmt const* s) { return ev.eval(s, r); }
    Control operator()(Break_stmt const* s) { return ev.eval(s, r); }
    Control operator()(Continue_stmt const* s) { return ev.eval(s, r); }
    Control operator()(Expression_stmt const* s) { return ev.eval(s, r); }
    Control operator()(Declaration_stmt const* s) { return ev.eval(s, r); }
  };

  return apply(s, Fn{*this, r});
}


Control
Evaluator::eval(Empty_stmt const* s, Value& r)
{
  return next_ctl;
}


Control
Evaluator::eval(Block_stmt const* s, Value& r)
{
  Store_sentinel store(*this);
  for(Stmt const* s1 : s->statements()) {

    // Evaluate each statement in turn. If the
    Control ctl = eval(s1, r);
    switch (ctl) {
      case return_ctl:
      case break_ctl:
      case continue_ctl:
        return ctl;
      default:
        break;
    }
  }
  return next_ctl;
}


Control
Evaluator::eval(Assign_stmt const* s, Value& r)
{
  Value lhs = eval(s->object());
  Value rhs = eval(s->value());
  *lhs.get_reference() = rhs;
  return next_ctl;
}


Control
Evaluator::eval(Return_stmt const* s, Value& r)
{
  r = eval(s->value());
  return return_ctl;
}


// If the condition evaluates to true, then the body
// is evaluated.
Control
Evaluator::eval(If_then_stmt const* s, Value& r)
{
  Value c = eval(s->condition());
  if (c.get_integer())
    return eval(s->body(), r);
  return next_ctl;
}


// If the condition evaluates to true, the true branch
// is evaluated. Otherwise the false branch is evaluated.
// Note that control stops if either branch returns,
// breaks, or continues. In all other cases, control
// flows to the next statement.
Control
Evaluator::eval(If_else_stmt const* s, Value& r)
{
  Value c = eval(s->condition());
  if (c.get_integer())
    return eval(s->true_branch(), r);
  else
    return eval(s->false_branch(), r);
}


// Continue evaluationg the body while the condition
// evaluates to true.
Control
Evaluator::eval(While_stmt const* s, Value& r)
{
  while (true) {
    Value c = eval(s->condition());
    if (!c.get_integer())
      break;

    // Evaluate the body. Stop iterating if we got
    // a break, or return if we got a return.
    // Otherwise, continue to the next iteration.
    Control ctl = eval(s->body(), r);
    if (ctl == break_ctl)
      break;
    if (ctl == return_ctl)
      return ctl;
  }
  return next_ctl;
}


Control
Evaluator::eval(Break_stmt const* s, Value& r)
{
  return break_ctl;
}


Control
Evaluator::eval(Continue_stmt const* s, Value& r)
{
  return continue_ctl;
}


Control
Evaluator::eval(Expression_stmt const* s, Value& r)
{
  eval(s->expression());
  return next_ctl;
}


Control
Evaluator::eval(Declaration_stmt const* s, Value& r)
{
  eval(s->declaration());
  return next_ctl;
}


// -------------------------------------------------------------------------- //
// Expression reduction

// Return a literal corresponding to the evaluation
// of e. If e invokes undefined behavior or is not
// otherwise a constant expression, this returns nullptr.
Expr*
reduce(Expr const* e)
{
  // If the expression is already a literal, then just
  // return it.
  if (is<Literal_expr>(e))
    return const_cast<Expr*>(e);

  // Otherwise, try evaluating.
  try {
    Value v = evaluate(e);
    return new Literal_expr(e->type(), v);
  } catch (...) {
    return nullptr;
  }
}


// -------------------------------------------------------------------------- //
// Program execution

// Execute the given function.
//
// TODO: What if there are operands?
Value
Evaluator::exec(Function_decl const* fn)
{
  // Evaluate all of the top-level declarations in
  // order to re-establish the evaluation context.
  Store_sentinel store(*this);
  Module_decl const* m = cast<Module_decl>(fn->context());
  for (Decl const* d : m->declarations())
    eval(d);

  // TODO: Check the result code.
  Value result;
  Control ctl = eval(fn->body(), result);
  if (ctl != return_ctl)
    throw std::runtime_error("function error");

  return result;
}
