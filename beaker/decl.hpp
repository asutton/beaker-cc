// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_DECL_HPP
#define BEAKER_DECL_HPP

#include "prelude.hpp"


// Represents the declaration of a named entity.
// Every declaration has a name and a type. Note that
// user-defined type declarations (e.g., modulues)
// have nullptr type. We use this to indicate a higher
// order type.
struct Decl
{
  struct Visitor;
  struct Mutator;

  Decl(Symbol const* s, Type const* t)
    : name_(s), type_(t)
  { }

  virtual ~Decl() { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  Decl const*   context() const { return cxt_; }
  Symbol const* name() const { return name_; }
  Type const*   type() const { return type_; }

  Decl const*   cxt_;
  Symbol const* name_;
  Type const*   type_;
};


// The read-only declaration visitor.
struct Decl::Visitor
{
  virtual void visit(Variable_decl const*) = 0;
  virtual void visit(Function_decl const*) = 0;
  virtual void visit(Parameter_decl const*) = 0;
  virtual void visit(Module_decl const*) = 0;
};


// The read/write declaration visitor.
struct Decl::Mutator
{
  virtual void visit(Variable_decl*) = 0;
  virtual void visit(Function_decl*) = 0;
  virtual void visit(Parameter_decl*) = 0;
  virtual void visit(Module_decl*) = 0;
};


// Represents variable declarations.
struct Variable_decl : Decl
{
  Variable_decl(Symbol const* n, Type const* t, Expr* e)
    : Decl(n, t), init_(e)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Expr const* init() const { return init_; }
  Expr*       init()       { return init_; }

  Expr* init_;
};


// Represents function declarations.
struct Function_decl : Decl
{
  Function_decl(Symbol const* n, Type const* t, Decl_seq const& p, Stmt* b)
    : Decl(n, t), parms_(p), body_(b)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Decl_seq const&      parameters() const { return parms_; }
  
  Function_type const* type() const;
  Type const*          return_type() const;

  Stmt const* body() const { return body_; }
  Stmt*       body()       { return body_; }

  Decl_seq parms_;
  Stmt*    body_;
};


// Represents parameter declarations.
struct Parameter_decl : Decl
{
  using Decl::Decl;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }
};


// A module is a sequence of top-level declarations.
struct Module_decl : Decl
{
  Module_decl(Symbol const* n, Decl_seq const& d)
    : Decl(n, nullptr), decls_(d)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Decl_seq const& declarations() const { return decls_; }

  Decl_seq decls_;
};



// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename R>
struct Generic_decl_visitor : Decl::Visitor
{
  Generic_decl_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Variable_decl const* d) { r = fn(d); }
  void visit(Function_decl const* d) { r = fn(d); }
  void visit(Parameter_decl const* d) { r = fn(d); }
  void visit(Module_decl const* d) { r = fn(d); }

  F fn;
  R r;
};


// A specialization for functions returning void.
template<typename F>
struct Generic_decl_visitor<F, void> : Decl::Visitor
{
  Generic_decl_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Variable_decl const* d) { fn(d); }
  void visit(Function_decl const* d) { fn(d); }
  void visit(Parameter_decl const* d) { fn(d); }
  void visit(Module_decl const* d) { fn(d); }

  F fn;
};


// Dispatch visitor to a void visitor.
template<typename F, typename R = typename std::result_of<F(Variable_decl const*)>::type>
inline typename std::enable_if<std::is_void<R>::value, void>::type
dispatch(Decl const* d, F fn)
{
  Generic_decl_visitor<F, void> v(fn);
  d->accept(v);
}


// Dispatch to a non-void visitor.
template<typename F, typename R = typename std::result_of<F(Variable_decl const*)>::type>
inline typename std::enable_if<!std::is_void<R>::value, R>::type
dispatch(Decl const* p, F fn)
{
  Generic_decl_visitor<F, R> v(fn);
  p->accept(v);
  return v.r;
}


// Apply fn to the propositoin p.
template<typename F, typename R = typename std::result_of<F(Variable_decl const*)>::type>
inline R
apply(Decl const* p, F fn)
{
  return dispatch(p, fn);
}


template<typename F, typename R>
struct Generic_decl_mutator : Decl::Mutator
{
  Generic_decl_mutator(F fn)
    : fn(fn)
  { }
  
  void visit(Variable_decl* d) { r = fn(d); }
  void visit(Function_decl* d) { r = fn(d); }
  void visit(Parameter_decl* d) { r = fn(d); }
  void visit(Module_decl* d) { r = fn(d); }

  F fn;
  R r;
};


// A specialization for functions returning void.
template<typename F>
struct Generic_decl_mutator<F, void> : Decl::Mutator
{
  Generic_decl_mutator(F fn)
    : fn(fn)
  { }
  
  void visit(Variable_decl* d) { fn(d); }
  void visit(Function_decl* d) { fn(d); }
  void visit(Parameter_decl* d) { fn(d); }
  void visit(Module_decl* d) { fn(d); }

  F fn;
};


// Dispatch visitor to a void visitor.
template<typename F, typename R = typename std::result_of<F(Variable_decl*)>::type>
inline typename std::enable_if<std::is_void<R>::value, void>::type
dispatch(Decl* d, F fn)
{
  Generic_decl_mutator<F, void> v(fn);
  d->accept(v);
}


// Dispatch to a non-void visitor.
template<typename F, typename R = typename std::result_of<F(Variable_decl*)>::type>
inline typename std::enable_if<!std::is_void<R>::value, R>::type
dispatch(Decl* p, F fn)
{
  Generic_decl_mutator<F, R> v(fn);
  p->accept(v);
  return v.r;
}


// Apply fn to the propositoin p.
template<typename F, typename R = typename std::result_of<F(Variable_decl*)>::type>
inline R
apply(Decl* p, F fn)
{
  return dispatch(p, fn);
}


#endif
