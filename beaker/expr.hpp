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

  Expr(Type const* t)
    : type_(t)
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
  virtual void visit(Value_conv const*) = 0;
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
  virtual void visit(Value_conv*) = 0;
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

  Symbol const* symbol() const   { return sym; }
  String const& spelling() const { return sym->spelling(); }

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

  Symbol const* symbol() const             { return sym; }
  String const& spelling() const           { return sym->spelling(); }

  Decl const*   declaration() const        { return decl; }
  void          declaration(Decl const* d) { decl = d; }

  Symbol const* sym;
  Decl const*   decl;
};


// A helper class  for unary expressions.
struct Unary_expr : Expr
{
  Unary_expr(Expr* e)
    : first(e)
  { }

  Expr* operand() const { return first; }

  Expr* first;
};


// A helper function for binary expressions.
struct Binary_expr : Expr
{
  Binary_expr(Expr* e1, Expr* e2)
    : first(e1), second(e2)
  { }

  Expr* left() const { return first; }
  Expr* right() const { return second; }

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
//                              Conversions

// Represents the conversion of a source expression to
// a target type.
struct Conversion : Expr
{
  Conversion(Type const* t, Expr* e)
    : Expr(t), first(e)
  { }

  Expr*       source() const { return first; }
  Type const* target() const { return type(); }

  Expr* first;
};


// Represents the conversion of an object to a value.
struct Value_conv : Conversion
{
  using Conversion::Conversion;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// -------------------------------------------------------------------------- //
// Generic visitor

template<typename F, typename T>
struct Generic_expr_visitor : Expr::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_expr_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }
  
  void visit(Literal_expr const* e) { this->invoke(e); }
  void visit(Id_expr const* e) { this->invoke(e); }
  void visit(Add_expr const* e) { this->invoke(e); }
  void visit(Sub_expr const* e) { this->invoke(e); }
  void visit(Mul_expr const* e) { this->invoke(e); }
  void visit(Div_expr const* e) { this->invoke(e); }
  void visit(Rem_expr const* e) { this->invoke(e); }
  void visit(Neg_expr const* e) { this->invoke(e); }
  void visit(Pos_expr const* e) { this->invoke(e); }
  void visit(Eq_expr const* e) { this->invoke(e); }
  void visit(Ne_expr const* e) { this->invoke(e); }
  void visit(Lt_expr const* e) { this->invoke(e); }
  void visit(Gt_expr const* e) { this->invoke(e); }
  void visit(Le_expr const* e) { this->invoke(e); }
  void visit(Ge_expr const* e) { this->invoke(e); }
  void visit(And_expr const* e) { this->invoke(e); }
  void visit(Or_expr const* e) { this->invoke(e); }
  void visit(Not_expr const* e) { this->invoke(e); }
  void visit(Call_expr const* e) { this->invoke(e); }
  void visit(Value_conv const* e) { this->invoke(e); }
};


template<typename F, typename T = typename std::result_of<F(Literal_expr const*)>::type>
inline T
apply(Expr const* e, F fn)
{
  Generic_expr_visitor<F, T> v(fn);
  return lingo::accept(e, v);
}


// -------------------------------------------------------------------------- //
// Generic mutator


template<typename F, typename T>
struct Generic_expr_mutator : Expr::Mutator, lingo::Generic_mutator<F, T>
{
  Generic_expr_mutator(F fn)
    : lingo::Generic_mutator<F, T>(fn)
  { }
  
  void visit(Literal_expr* e) { this->invoke(e); }
  void visit(Id_expr* e) { this->invoke(e); }
  void visit(Add_expr* e) { this->invoke(e); }
  void visit(Sub_expr* e) { this->invoke(e); }
  void visit(Mul_expr* e) { this->invoke(e); }
  void visit(Div_expr* e) { this->invoke(e); }
  void visit(Rem_expr* e) { this->invoke(e); }
  void visit(Neg_expr* e) { this->invoke(e); }
  void visit(Pos_expr* e) { this->invoke(e); }
  void visit(Eq_expr* e) { this->invoke(e); }
  void visit(Ne_expr* e) { this->invoke(e); }
  void visit(Lt_expr* e) { this->invoke(e); }
  void visit(Gt_expr* e) { this->invoke(e); }
  void visit(Le_expr* e) { this->invoke(e); }
  void visit(Ge_expr* e) { this->invoke(e); }
  void visit(And_expr* e) { this->invoke(e); }
  void visit(Or_expr* e) { this->invoke(e); }
  void visit(Not_expr* e) { this->invoke(e); }
  void visit(Call_expr* e) { this->invoke(e); }
  void visit(Value_conv* e) { this->invoke(e); }
};


template<typename F, typename T = typename std::result_of<F(Literal_expr*)>::type>
inline T
apply(Expr* e, F fn)
{
  Generic_expr_mutator<F, T> v(fn);
  return lingo::accept(e, v);
}



#endif
