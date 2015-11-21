// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_DIRECTIVE_HPP
#define BEAKER_DIRECTIVE_HPP

// The top-level representation of a program is a sequence
// of directives.

#include "prelude.hpp"


// Represents the declaration of a named entity.
// Every declaration has a name and a type. Note that
// user-defined type declarations (e.g., modulues)
// have nullptr type. We use this to indicate a higher
// order type.
struct Directive
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;
};


// The read-only declaration visitor.
struct Directive::Visitor
{
  virtual void visit(Module_dir const*) = 0;
  virtual void visit(Import_dir const*) = 0;
  virtual void visit(Declaration_dir const*) = 0;
};


// The read/write declaration visitor.
struct Directive::Mutator
{
  virtual void visit(Module_dir*) = 0;
  virtual void visit(Import_dir*) = 0;
  virtual void visit(Declaration_dir*) = 0;
};


// Delcares the current translation to be a module.
struct Module_dir : Directive
{
  Module_dir(Expr* n)
    : first(n)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Expr* name() const { return first; }

  Expr* first;
};


// Delcares a dependence on the given module.
struct Import_dir : Directive
{
  Import_dir(Expr* n)
    : first(n)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Expr* name() const { return first; }

  Expr* first;
};


// Declares a top-level entity within the
// translation unit.
struct Declaration_dir : Directive
{
  Declaration_dir(Decl* d)
    : first(d)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Decl* declaration() const { return first; }

  Decl* first;
};


// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename T>
struct Generic_dir_visitor : Directive::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_dir_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }

  void visit(Module_dir const* d) { this->invoke(d); }
  void visit(Import_dir const* d) { this->invoke(d); }
  void visit(Declaration_dir const* d) { this->invoke(d); }
};


// Apply fn to the declaration d.
template<typename F, typename T = typename std::result_of<F(Module_dir const*)>::type>
inline T
apply(Directive const* d, F fn)
{
  Generic_dir_visitor<F, T> v = fn;
  return accept(d, v);
}


template<typename F, typename T>
struct Generic_dir_mutator : Directive::Mutator, lingo::Generic_mutator<F, T>
{
  Generic_dir_mutator(F fn)
    : lingo::Generic_mutator<F, T>(fn)
  { }

  void visit(Module_dir* d) { this->invoke(d); }
  void visit(Import_dir* d) { this->invoke(d); }
  void visit(Declaration_dir* d) { this->invoke(d); }
};


// Apply fn to the propositoin p.
template<typename F, typename T = typename std::result_of<F(Module_dir*)>::type>
inline T
apply(Directive* d, F fn)
{
  Generic_dir_mutator<F, T> v = fn;
  return accept(d, v);
}


#endif
