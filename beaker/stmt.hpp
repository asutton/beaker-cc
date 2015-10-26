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
  virtual void visit(Expression_stmt const*) = 0;
  virtual void visit(Declaration_stmt const*) = 0;
};


// The read/write declaration visitor.
struct Stmt::Mutator
{
  virtual void visit(Empty_stmt*) = 0;
  virtual void visit(Block_stmt*) = 0;
  virtual void visit(Return_stmt*) = 0;
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

  Expr const* value() const { return first; }
  Expr*       value()       { return first; }

  Expr* first;
};


// An expression statement.
struct Expression_stmt : Stmt
{
  Expression_stmt(Expr* e)
    : first(e)
  { }

  void accept(Visitor& v) const { return v.visit(this); }
  void accept(Mutator& v)       { return v.visit(this); }

  Expr const* expression() const { return first; }
  Expr*       expression()       { return first; }

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

  Decl const* declaration() const { return first; }
  Decl*       declaration()       { return first; }

  Decl* first;
};


// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename R>
struct Generic_stmt_visitor : Stmt::Visitor
{
  Generic_stmt_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Empty_stmt const* d) { r = fn(d); };
  void visit(Block_stmt const* d) { r = fn(d); };
  void visit(Return_stmt const* d) { r = fn(d); };
  void visit(Expression_stmt const* d) { r = fn(d); };
  void visit(Declaration_stmt const* d) { r = fn(d); };

  F fn;
  R r;
};


// A specialization for functions returning void.
template<typename F>
struct Generic_stmt_visitor<F, void> : Stmt::Visitor
{
  Generic_stmt_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Empty_stmt const* d) { fn(d); };
  void visit(Block_stmt const* d) { fn(d); };
  void visit(Return_stmt const* d) { fn(d); };
  void visit(Expression_stmt const* d) { fn(d); };
  void visit(Declaration_stmt const* d) { fn(d); };

  F fn;
};


// Dispatch visitor to a void visitor.
template<typename F, typename R = typename std::result_of<F(Empty_stmt const*)>::type>
inline typename std::enable_if<std::is_void<R>::value, void>::type
dispatch(Stmt const* s, F fn)
{
  Generic_stmt_visitor<F, void> v(fn);
  s->accept(v);
}


// Dispatch to a non-void visitor.
template<typename F, typename R = typename std::result_of<F(Empty_stmt const*)>::type>
inline typename std::enable_if<!std::is_void<R>::value, R>::type
dispatch(Stmt const* s, F fn)
{
  Generic_stmt_visitor<F, R> v(fn);
  s->accept(v);
  return v.r;
}


// Apply fn to the propositoin p.
template<typename F, typename R = typename std::result_of<F(Empty_stmt const*)>::type>
inline R
apply(Stmt const* s, F fn)
{
  return dispatch(s, fn);
}


template<typename F, typename R>
struct Generic_stmt_mutator : Stmt::Mutator
{
  Generic_stmt_mutator(F fn)
    : fn(fn)
  { }
  
  void visit(Empty_stmt* d) { r = fn(d); };
  void visit(Block_stmt* d) { r = fn(d); };
  void visit(Return_stmt* d) { r = fn(d); };
  void visit(Expression_stmt* d) { r = fn(d); };
  void visit(Declaration_stmt* d) { r = fn(d); };

  F fn;
  R r;
};


// A specialization for functions returning void.
template<typename F>
struct Generic_stmt_mutator<F, void> : Stmt::Mutator
{
  Generic_stmt_mutator(F fn)
    : fn(fn)
  { }
  
  void visit(Empty_stmt* d) { fn(d); };
  void visit(Block_stmt* d) { fn(d); };
  void visit(Return_stmt* d) { fn(d); };
  void visit(Expression_stmt* d) { fn(d); };
  void visit(Declaration_stmt* d) { fn(d); };

  F fn;
};


// Dispatch visitor to a void visitor.
template<typename F, typename R = typename std::result_of<F(Empty_stmt*)>::type>
inline typename std::enable_if<std::is_void<R>::value, void>::type
dispatch(Stmt* s, F fn)
{
  Generic_stmt_mutator<F, void> v(fn);
  s->accept(v);
}


// Dispatch to a non-void visitor.
template<typename F, typename R = typename std::result_of<F(Empty_stmt*)>::type>
inline typename std::enable_if<!std::is_void<R>::value, R>::type
dispatch(Stmt* s, F fn)
{
  Generic_stmt_mutator<F, R> v(fn);
  s->accept(v);
  return v.r;
}


// Apply fn to the propositoin p.
template<typename F, typename R = typename std::result_of<F(Empty_stmt*)>::type>
inline R
apply(Stmt* s, F fn)
{
  return dispatch(s, fn);
}


#endif
