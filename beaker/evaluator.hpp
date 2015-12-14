// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EVALUATOR_HPP
#define BEAKER_EVALUATOR_HPP

#include <beaker/prelude.hpp>
#include <beaker/value.hpp>
#include <beaker/environment.hpp>


// Dynamic binding of symbols to their values.
using Store = Environment<Symbol const*, Value>;


// The store stack maintains the dynamic binding
// between symbols and their values. Each time
// a new binding environment is entered (e.g., a
// block scope), a new frame is created to store
// bindings.
//
// This is also the call stack.
using Store_stack = Stack<Store>;


// Represents the evaluation of a statement.
// This determines the next action to be
// taken.
enum Control
{
  next_ctl,
  return_ctl,
  break_ctl,
  continue_ctl,
};


// The evaluator is responsible for the interpretation
// of a program as a value.
class Evaluator
{
  struct Store_sentinel;
public:
  Value eval(Expr const*);
  Value eval(Literal_expr const*);
  Value eval(Id_expr const*);
  Value eval(Decl_expr const*);
  Value eval(Add_expr const*);
  Value eval(Sub_expr const*);
  Value eval(Mul_expr const*);
  Value eval(Div_expr const*);
  Value eval(Rem_expr const*);
  Value eval(Neg_expr const*);
  Value eval(Pos_expr const*);
  Value eval(Eq_expr const*);
  Value eval(Ne_expr const*);
  Value eval(Lt_expr const*);
  Value eval(Gt_expr const*);
  Value eval(Le_expr const*);
  Value eval(Ge_expr const*);
  Value eval(And_expr const*);
  Value eval(Or_expr const*);
  Value eval(Not_expr const*);
  Value eval(Call_expr const*);
  Value eval(Dot_expr const*);
  Value eval(Field_expr const*);
  Value eval(Method_expr const*);
  Value eval(Index_expr const*);
  Value eval(Value_conv const*);
  Value eval(Block_conv const*);

  void eval_init(Expr const*, Value&);
  void eval_init(Default_init const*, Value&);
  void eval_init(Trivial_init const*, Value&);
  void eval_init(Copy_init const*, Value&);
  void eval_init(Reference_init const*, Value&);

  void eval(Decl const*);
  void eval(Variable_decl const*);
  void eval(Function_decl const*);
  void eval(Parameter_decl const*);
  void eval(Record_decl const*);
  void eval(Field_decl const*);
  void eval(Method_decl const*);
  void eval(Module_decl const*);

  Control eval(Stmt const*, Value&);
  Control eval(Empty_stmt const*, Value&);
  Control eval(Block_stmt const*, Value&);
  Control eval(Assign_stmt const*, Value&);
  Control eval(Return_stmt const*, Value&);
  Control eval(If_then_stmt const*, Value&);
  Control eval(If_else_stmt const*, Value&);
  Control eval(While_stmt const*, Value&);
  Control eval(Break_stmt const*, Value&);
  Control eval(Continue_stmt const*, Value&);
  Control eval(Expression_stmt const*, Value&);
  Control eval(Declaration_stmt const*, Value&);

  Value exec(Function_decl const*);

private:
  Store_stack stack;
};


// A helper class for managing stack frames.
struct Evaluator::Store_sentinel
{
  Store_sentinel(Evaluator& e)
    : eval(e)
  {
    eval.stack.push();
  }

  ~Store_sentinel()
  {
    eval.stack.pop();
  }

  Evaluator& eval;
};


// -------------------------------------------------------------------------- //
// Expression evaluation

// Evaluate the given expression.
inline Value
evaluate(Expr const* e)
{
  Evaluator ev;
  return ev.eval(e);
}

Expr* reduce(Expr const* e);


#endif
