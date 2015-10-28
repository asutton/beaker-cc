// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_STMT_HPP
#define BEAKER_STMT_HPP


// The base class of all statements in the language.
struct Stmt
{
  struct Visitor;
  struct Mutator;

  virtual ~Stmt() { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;
};


// The read-only declaration visitor.
struct Stmt::Visitor
{
  virtual void visit(Empty_stmt const*) = 0;
  virtual void visit(Block_stmt const*) = 0;
  virtual void visit(Return_stmt const*) = 0;
  virtual void visit(If_then_stmt const*) = 0;
  virtual void visit(If_else_stmt const*) = 0;
  virtual void visit(Expression_stmt const*) = 0;
  virtual void visit(Declaration_stmt const*) = 0;
};


// The read/write declaration visitor.
struct Stmt::Mutator
{
  virtual void visit(Empty_stmt*) = 0;
  virtual void visit(Block_stmt*) = 0;
  virtual void visit(Return_stmt*) = 0;
  virtual void visit(If_then_stmt*) = 0;
  virtual void visit(If_else_stmt*) = 0;
  virtual void visit(Expression_stmt*) = 0;
  virtual void visit(Declaration_stmt*) = 0;
};



// The empty statement.
struct Empty_stmt : Stmt
{
  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }
};


// A block statement.
struct Block_stmt : Stmt
{
  Block_stmt(Stmt_seq const& s)
    : first(s)
  { }

  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }

  Stmt_seq const& statements() const { return first; }

  Stmt_seq first;
};


// A return statement.
struct Return_stmt : Stmt
{
  Return_stmt(Expr* e)
    : first(e)
  { }

  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }

  Expr* value() const { return first; }

  Expr* first;
};


// A statement of the form:
//
//    if (e) s
struct If_then_stmt : Stmt
{
  If_then_stmt(Expr* e, Stmt* s)
    : first(e), second(s)
  { }

  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }

  Expr* condition() const { return first; }
  Stmt* body() const { return second; }

  Expr* first;
  Stmt* second;
};


// A statement of the form:
//
//    if (e) s1 else s2
struct If_else_stmt : Stmt
{
  If_else_stmt(Expr* e, Stmt* s1, Stmt* s2)
    : first(e), second(s1), third(s2)
  { }

  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }

  Expr* condition() const    { return first; }
  Stmt* true_branch() const  { return second; }
  Stmt* false_branch() const { return third; }

  Expr* first;
  Stmt* second;
  Stmt* third;
};


// An expression statement.
struct Expression_stmt : Stmt
{
  Expression_stmt(Expr* e)
    : first(e)
  { }

  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }

  Expr* expression() const { return first; }

  Expr* first;
};


// A declaration statement.
struct Declaration_stmt : Stmt
{
  Declaration_stmt(Decl* d)
    : first(d)
  { }

  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }

  Decl* declaration() const { return first; }

  Decl* first;
};


// -------------------------------------------------------------------------- //
// Generic visitors

template<typename F, typename T>
struct Generic_stmt_visitor : Stmt::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_stmt_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }
  
  void visit(Empty_stmt const* d) { this->invoke(d); };
  void visit(Block_stmt const* d) { this->invoke(d); };
  void visit(Return_stmt const* d) { this->invoke(d); };
  void visit(If_then_stmt const* d) { this->invoke(d); };
  void visit(If_else_stmt const* d) { this->invoke(d); };
  void visit(Expression_stmt const* d) { this->invoke(d); };
  void visit(Declaration_stmt const* d) { this->invoke(d); };
};


// Apply fn to the propositoin p.
template<typename F, typename T = typename std::result_of<F(Empty_stmt const*)>::type>
inline T
apply(Stmt const* s, F fn)
{
  Generic_stmt_visitor<F, T> v(fn);
  return lingo::accept(s, v);
}


// -------------------------------------------------------------------------- //
// Generic mutator


template<typename F, typename T>
struct Generic_stmt_mutator : Stmt::Mutator, lingo::Generic_mutator<F, T>
{
  Generic_stmt_mutator(F fn)
    : lingo::Generic_mutator<F, T>(fn)
  { }
  
  void visit(Empty_stmt* d) { this->invoke(d); };
  void visit(Block_stmt* d) { this->invoke(d); };
  void visit(Return_stmt* d) { this->invoke(d); };
  void visit(If_then_stmt* d) { this->invoke(d); };
  void visit(If_else_stmt* d) { this->invoke(d); };
  void visit(Expression_stmt* d) { this->invoke(d); };
  void visit(Declaration_stmt* d) { this->invoke(d); };
};


// Apply fn to the propositoin p.
template<typename F, typename T = typename std::result_of<F(Empty_stmt*)>::type>
inline T
apply(Stmt* s, F fn)
{
  Generic_stmt_mutator<F, T> m(fn);
  return lingo::accept(s, m);
}


#endif
