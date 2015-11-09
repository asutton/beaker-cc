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
  virtual void visit(Record_decl const*) = 0;
  virtual void visit(Field_decl const*) = 0;
  virtual void visit(Module_decl const*) = 0;
};


// The read/write declaration visitor.
struct Decl::Mutator
{
  virtual void visit(Variable_decl*) = 0;
  virtual void visit(Function_decl*) = 0;
  virtual void visit(Parameter_decl*) = 0;
  virtual void visit(Record_decl*) = 0;
  virtual void visit(Field_decl*) = 0;
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


// Declares a user-defined record type.
struct Record_decl : Decl
{
  Record_decl(Symbol const* n, Decl_seq const& f)
    : Decl(n, nullptr), fields_(f)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Decl_seq const& fields() const { return fields_; }

  Decl_seq fields_;
};


// A member of a record.
struct Field_decl : Decl
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
// Queries

// Returns true if v is a global variable.
inline bool
is_global_variable(Variable_decl const* v)
{
  return is<Module_decl>(v->context());
}


// Returns true if v is a local variable.
//
// TODO: This actually depends more on storage properties
// than on declaration context. For example, if the language
// allowed static local variables (as in C++), then this
// would also need to check for an appropriate declaration
// specifier.
inline bool
is_local_variable(Variable_decl const* v)
{
  return is<Function_decl>(v->context());
}


// Returns true if the declaration defines an object.
// Only variables, parameters, and fields define objects.
inline bool
defines_object(Decl const* d)
{
  return is<Variable_decl>(d)
      || is<Parameter_decl>(d)
      || is<Field_decl>(d);
}


// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename T>
struct Generic_decl_visitor : Decl::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_decl_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }

  void visit(Variable_decl const* d) { this->invoke(d); }
  void visit(Function_decl const* d) { this->invoke(d); }
  void visit(Parameter_decl const* d) { this->invoke(d); }
  void visit(Record_decl const* d) { this->invoke(d); }
  void visit(Field_decl const* d) { this->invoke(d); }
  void visit(Module_decl const* d) { this->invoke(d); }
};


// Apply fn to the declaration d.
template<typename F, typename T = typename std::result_of<F(Variable_decl const*)>::type>
inline T
apply(Decl const* d, F fn)
{
  Generic_decl_visitor<F, T> v = fn;
  return accept(d, v);
}


template<typename F, typename T>
struct Generic_decl_mutator : Decl::Mutator, lingo::Generic_mutator<F, T>
{
  Generic_decl_mutator(F fn)
    : lingo::Generic_mutator<F, T>(fn)
  { }

  void visit(Variable_decl* d) { this->invoke(d); }
  void visit(Function_decl* d) { this->invoke(d); }
  void visit(Parameter_decl* d) { this->invoke(d); }
  void visit(Record_decl* d) { this->invoke(d); }
  void visit(Field_decl* d) { this->invoke(d); }
  void visit(Module_decl* d) { this->invoke(d); }
};


// Apply fn to the propositoin p.
template<typename F, typename T = typename std::result_of<F(Variable_decl*)>::type>
inline T
apply(Decl* d, F fn)
{
  Generic_decl_mutator<F, T> v = fn;
  return accept(d, v);
}


#endif
