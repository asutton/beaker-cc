// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_DECL_HPP
#define BEAKER_DECL_HPP

#include "prelude.hpp"
#include "scope.hpp"
#include "specifier.hpp"


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
    : spec_(no_spec), name_(s), type_(t), cxt_(nullptr)
  { }

  Decl(Specifier spec, Symbol const* s, Type const* t)
    : spec_(spec), name_(s), type_(t), cxt_(nullptr)
  { }

  virtual ~Decl() { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  // Declaration specifiers
  Specifier specifiers() const { return spec_; }
  bool      is_foreign() const { return spec_ & foreign_spec; }

  Symbol const* name() const { return name_; }
  Type const*   type() const { return type_; }

  Decl const*        context() const { return cxt_; }
  Module_decl const* module() const;

  Specifier     spec_;
  Symbol const* name_;
  Type const*   type_;
  Decl const*   cxt_;
};


// The read-only declaration visitor.
struct Decl::Visitor
{
  virtual void visit(Variable_decl const*) = 0;
  virtual void visit(Function_decl const*) = 0;
  virtual void visit(Parameter_decl const*) = 0;
  virtual void visit(Record_decl const*) = 0;
  virtual void visit(Field_decl const*) = 0;
  virtual void visit(Method_decl const*) = 0;
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
  virtual void visit(Method_decl*) = 0;
  virtual void visit(Module_decl*) = 0;
};


// Represents variable declarations.
struct Variable_decl : Decl
{
  Variable_decl(Symbol const* n, Type const* t, Expr* e)
    : Decl(n, t), init_(e)
  { }

  Variable_decl(Specifier spec, Symbol const* n, Type const* t, Expr* e)
    : Decl(spec, n, t), init_(e)
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

  Function_decl(Specifier spec, Symbol const* n, Type const* t, Decl_seq const& p, Stmt* b)
    : Decl(spec, n, t), parms_(p), body_(b)
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
//
// The record class maintains two sets of declarations:
// fields, which constitute its actual type, and
// another set of member declarations (e.g., methods,
// nested types, templates, constants, etc). These
// aren't really part of the object, just part of
// the scope.
//
// A record declaration defines a scope. Declarations
// within the record are cached here for use during
// member lookup.
struct Record_decl : Decl
{
  Record_decl(Symbol const* n, Decl_seq const& f, Decl_seq const& m)
    : Decl(n, nullptr), fields_(f), members_(m), scope_(this)
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Decl_seq const& fields() const { return fields_; }
  Decl_seq const& members() const { return members_; }

  Scope*          scope()       { return &scope_; }
  Scope const*    scope() const { return &scope_; }

  Decl_seq fields_;
  Decl_seq members_;
  Scope    scope_;
};


// A member variable of a record.
//
// TODO: Cache the field index?
struct Field_decl : Decl
{
  using Decl::Decl;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Record_decl const* context() const { return cast<Record_decl>(cxt_); }

  int index() const;
};


// A member function of a record. A member function of
// a record T has an implicit parameter named 'this' whose
// type is T&.
struct Method_decl : Function_decl
{
  using Function_decl::Function_decl;

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Record_decl const* context() const { return cast<Record_decl>(cxt_); }
};


// A module is a sequence of top-level directives.
struct Module_decl : Decl
{
  Module_decl(Symbol const* n, Dir_seq const& d)
    : Decl(n, nullptr), dirs_(d), mod_()
  { }

  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Dir_seq const& directives() const { return dirs_; }

  Expr* module() const { return mod_; }

  Dir_seq dirs_; // List of directives
  Expr*   mod_;  // An optional qualified module name
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
is_object(Decl const* d)
{
  return is<Variable_decl>(d)
      || is<Parameter_decl>(d)
      || is<Field_decl>(d);
}


// Returns true if the declaration is a reference.
bool is_reference(Decl const*);


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
  void visit(Method_decl const* d) { this->invoke(d); }
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
  void visit(Method_decl* d) { this->invoke(d); }
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
