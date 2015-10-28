// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EXPR_HPP
#define BEAKER_EXPR_HPP

#include "prelude.hpp"
#include "symbol.hpp"


// The Expr class represents the set of all expressions
// that defined by the language.
//
// Note that every expression has a type. The type is
// not initialized during parsing, but during elaboration.
struct Expr
{
  struct Visitor;
  struct Mutator;

  Expr()
    : type_(nullptr)
  { }

  virtual ~Expr() { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  Type const* type() const        { return type_; }
  void        type(Type const* t) { type_ = t; }

  Type const* type_;
};


// The base class of all non-modifiying visitors
// of the expression tree.
struct Expr::Visitor
{
  virtual void visit(Literal_expr const*) = 0;
  virtual void visit(Id_expr const*) = 0;
  virtual void visit(Add_expr const*) = 0;
  virtual void visit(Sub_expr const*) = 0;
  virtual void visit(Mul_expr const*) = 0;
  virtual void visit(Div_expr const*) = 0;
  virtual void visit(Rem_expr const*) = 0;
  virtual void visit(Neg_expr const*) = 0;
  virtual void visit(Pos_expr const*) = 0;
  virtual void visit(Eq_expr const*) = 0;
  virtual void visit(Ne_expr const*) = 0;
  virtual void visit(Lt_expr const*) = 0;
  virtual void visit(Gt_expr const*) = 0;
  virtual void visit(Le_expr const*) = 0;
  virtual void visit(Ge_expr const*) = 0;
  virtual void visit(And_expr const*) = 0;
  virtual void visit(Or_expr const*) = 0;
  virtual void visit(Not_expr const*) = 0;
  virtual void visit(Call_expr const*) = 0;
};


// The base class of all modifying visitors of
// the expression tree.
struct Expr::Mutator
{
  virtual void visit(Literal_expr*) = 0;
  virtual void visit(Id_expr*) = 0;
  virtual void visit(Add_expr*) = 0;
  virtual void visit(Sub_expr*) = 0;
  virtual void visit(Mul_expr*) = 0;
  virtual void visit(Div_expr*) = 0;
  virtual void visit(Rem_expr*) = 0;
  virtual void visit(Neg_expr*) = 0;
  virtual void visit(Pos_expr*) = 0;
  virtual void visit(Eq_expr*) = 0;
  virtual void visit(Ne_expr*) = 0;
  virtual void visit(Lt_expr*) = 0;
  virtual void visit(Gt_expr*) = 0;
  virtual void visit(Le_expr*) = 0;
  virtual void visit(Ge_expr*) = 0;
  virtual void visit(And_expr*) = 0;
  virtual void visit(Or_expr*) = 0;
  virtual void visit(Not_expr*) = 0;
  virtual void visit(Call_expr*) = 0;
};


// A boolean or integer literal value. The kind
// of literal is determined by the underlying symbol.
//
// TODO: Can we ever have a function literal? That
// would basically be an unnamed function object
// since functions can only ever be named.
struct Literal_expr : Expr
{
  Literal_expr(Symbol const* s)
    : sym(s)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Symbol const* symbol() const { return sym; }

  Symbol const* sym;
};


// An identifier that refers to a declaration.
//
// The declaration, like the expression's type, is
// dsetermine during elaboration.
struct Id_expr : Expr
{
  Id_expr(Symbol const* s)
    : sym(s)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Symbol const* symbol() const { return sym; }
  Decl const*   declaration() const { return decl; }

  Symbol const* sym;
  Decl const*   decl;
};


// A helper class  for unary expressions.
struct Unary_expr : Expr
{
  Unary_expr(Expr* e)
    : first(e)
  { }

  Expr const* operand() const { return first; }

  Expr* first;
};


// A helper function for binary expressions.
struct Binary_expr : Expr
{
  Binary_expr(Expr* e1, Expr* e2)
    : first(e1), second(e2)
  { }

  Expr const* left() const { return first; }
  Expr const* right() const { return second; }

  Expr* first;
  Expr* second;
};


// The expression e1 + e2.
struct Add_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 - e2.
struct Sub_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 * e2.
struct Mul_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 / e2.
struct Div_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 % e2.
struct Rem_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression -e.
struct Neg_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression +e.
struct Pos_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 == e2.
struct Eq_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 != e2.
struct Ne_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 < e2.
struct Lt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 > e2.
struct Gt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 <= e2.
struct Le_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 >= e2.
struct Ge_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 && e2.
struct And_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression e1 || e2.
struct Or_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The expression !e.
struct Not_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// The express e(e1, e2, ..., en)
struct Call_expr : Expr
{
  Call_expr(Expr* f, Expr_seq const& a)
    : first(f), second(a)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Expr const* target() const { return first; }
  Expr*       target()       { return first; }

  Expr_seq const& arguments() const { return second; }
  Expr_seq&       arguments()       { return second; }

  Expr*    first;
  Expr_seq second;
};


// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename R>
struct Generic_expr_visitor : Expr::Visitor
{
  Generic_expr_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Literal_expr const* e) { r = fn(e); }
  void visit(Id_expr const* e) { r = fn(e); }
  void visit(Add_expr const* e) { r = fn(e); }
  void visit(Sub_expr const* e) { r = fn(e); }
  void visit(Mul_expr const* e) { r = fn(e); }
  void visit(Div_expr const* e) { r = fn(e); }
  void visit(Rem_expr const* e) { r = fn(e); }
  void visit(Neg_expr const* e) { r = fn(e); }
  void visit(Pos_expr const* e) { r = fn(e); }
  void visit(Eq_expr const* e) { r = fn(e); }
  void visit(Ne_expr const* e) { r = fn(e); }
  void visit(Lt_expr const* e) { r = fn(e); }
  void visit(Gt_expr const* e) { r = fn(e); }
  void visit(Le_expr const* e) { r = fn(e); }
  void visit(Ge_expr const* e) { r = fn(e); }
  void visit(And_expr const* e) { r = fn(e); }
  void visit(Or_expr const* e) { r = fn(e); }
  void visit(Not_expr const* e) { r = fn(e); }
  void visit(Call_expr const* e) { r = fn(e); }

  F fn;
  R r;
};


// A specialization for functions returning void.
template<typename F>
struct Generic_expr_visitor<F, void> : Expr::Visitor
{
  Generic_expr_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Literal_expr const* e) { fn(e); }
  void visit(Id_expr const* e) { fn(e); }
  void visit(Add_expr const* e) { fn(e); }
  void visit(Sub_expr const* e) { fn(e); }
  void visit(Mul_expr const* e) { fn(e); }
  void visit(Div_expr const* e) { fn(e); }
  void visit(Rem_expr const* e) { fn(e); }
  void visit(Neg_expr const* e) { fn(e); }
  void visit(Pos_expr const* e) { fn(e); }
  void visit(Eq_expr const* e) { fn(e); }
  void visit(Ne_expr const* e) { fn(e); }
  void visit(Lt_expr const* e) { fn(e); }
  void visit(Gt_expr const* e) { fn(e); }
  void visit(Le_expr const* e) { fn(e); }
  void visit(Ge_expr const* e) { fn(e); }
  void visit(And_expr const* e) { fn(e); }
  void visit(Or_expr const* e) { fn(e); }
  void visit(Not_expr const* e) { fn(e); }
  void visit(Call_expr const* e) { fn(e); }

  F fn;
};


// Dispatch visitor to a void visitor.
template<typename F, typename R = typename std::result_of<F(Literal_expr const*)>::type>
inline typename std::enable_if<std::is_void<R>::value, void>::type
dispatch(Expr const* p, F fn)
{
  Generic_expr_visitor<F, void> v(fn);
  p->accept(v);
}


// Dispatch to a non-void visitor.
template<typename F, typename R = typename std::result_of<F(Literal_expr const*)>::type>
inline typename std::enable_if<!std::is_void<R>::value, R>::type
dispatch(Expr const* p, F fn)
{
  Generic_expr_visitor<F, R> v(fn);
  p->accept(v);
  return v.r;
}


// Apply fn to the propositoin p.
template<typename F, typename R = typename std::result_of<F(Literal_expr const*)>::type>
inline R
apply(Expr const* p, F fn)
{
  return dispatch(p, fn);
}


template<typename F, typename R>
struct Generic_mutator : Expr::Mutator
{
  Generic_mutator(F fn)
    : fn(fn)
  { }
  
  void visit(Literal_expr* e) { r = fn(e); }
  void visit(Id_expr* e) { r = fn(e); }
  void visit(Add_expr* e) { r = fn(e); }
  void visit(Sub_expr* e) { r = fn(e); }
  void visit(Mul_expr* e) { r = fn(e); }
  void visit(Div_expr* e) { r = fn(e); }
  void visit(Rem_expr* e) { r = fn(e); }
  void visit(Neg_expr* e) { r = fn(e); }
  void visit(Pos_expr* e) { r = fn(e); }
  void visit(Eq_expr* e) { r = fn(e); }
  void visit(Ne_expr* e) { r = fn(e); }
  void visit(Lt_expr* e) { r = fn(e); }
  void visit(Gt_expr* e) { r = fn(e); }
  void visit(Le_expr* e) { r = fn(e); }
  void visit(Ge_expr* e) { r = fn(e); }
  void visit(And_expr* e) { r = fn(e); }
  void visit(Or_expr* e) { r = fn(e); }
  void visit(Not_expr* e) { r = fn(e); }
  void visit(Call_expr* e) { r = fn(e); }

  F fn;
  R r;
};


// -------------------------------------------------------------------------- //
//                              Generic mutator


// A specialization for functions returning void.
template<typename F>
struct Generic_mutator<F, void> : Expr::Mutator
{
  Generic_mutator(F fn)
    : fn(fn)
  { }
  
  void visit(Literal_expr* e) { fn(e); }
  void visit(Id_expr* e) { fn(e); }
  void visit(Add_expr* e) { fn(e); }
  void visit(Sub_expr* e) { fn(e); }
  void visit(Mul_expr* e) { fn(e); }
  void visit(Div_expr* e) { fn(e); }
  void visit(Rem_expr* e) { fn(e); }
  void visit(Neg_expr* e) { fn(e); }
  void visit(Pos_expr* e) { fn(e); }
  void visit(Eq_expr* e) { fn(e); }
  void visit(Ne_expr* e) { fn(e); }
  void visit(Lt_expr* e) { fn(e); }
  void visit(Gt_expr* e) { fn(e); }
  void visit(Le_expr* e) { fn(e); }
  void visit(Ge_expr* e) { fn(e); }
  void visit(And_expr* e) { fn(e); }
  void visit(Or_expr* e) { fn(e); }
  void visit(Not_expr* e) { fn(e); }
  void visit(Call_expr* e) { fn(e); }

  F fn;
};


// Dispatch visitor to a void visitor.
template<typename F, typename R = typename std::result_of<F(Literal_expr*)>::type>
inline typename std::enable_if<std::is_void<R>::value, void>::type
dispatch(Expr* p, F fn)
{
  Generic_mutator<F, void> v(fn);
  p->accept(v);
}


// Dispatch to a non-void visitor.
template<typename F, typename R = typename std::result_of<F(Literal_expr*)>::type>
inline typename std::enable_if<!std::is_void<R>::value, R>::type
dispatch(Expr* p, F fn)
{
  Generic_mutator<F, R> v(fn);
  p->accept(v);
  return v.r;
}


// Apply fn to the propositoin p.
template<typename F, typename R = typename std::result_of<F(Literal_expr*)>::type>
inline R
apply(Expr* p, F fn)
{
  return dispatch(p, fn);
}


#endif
