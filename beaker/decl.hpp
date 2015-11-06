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
  virtual void visit(Record_decl const*) = 0;
  virtual void visit(Variable_decl const*) = 0;
  virtual void visit(Function_decl const*) = 0;
  virtual void visit(Parameter_decl const*) = 0;
  virtual void visit(Module_decl const*) = 0;

  // network declarations
  virtual void visit(Decode_decl const*) = 0;
  virtual void visit(Table_decl const*) = 0;
  virtual void visit(Flow_decl const*) = 0;
  virtual void visit(Port_decl const*) = 0;
  virtual void visit(Extracts_decl const*) = 0;
  virtual void visit(Rebind_decl const*) = 0;
};


// The read/write declaration visitor.
struct Decl::Mutator
{
  virtual void visit(Record_decl*) = 0;
  virtual void visit(Variable_decl*) = 0;
  virtual void visit(Function_decl*) = 0;
  virtual void visit(Parameter_decl*) = 0;
  virtual void visit(Module_decl*) = 0;

  // network declarations
  virtual void visit(Decode_decl*) = 0;
  virtual void visit(Table_decl*) = 0;
  virtual void visit(Flow_decl*) = 0;
  virtual void visit(Port_decl*) = 0;
  virtual void visit(Extracts_decl*) = 0;
  virtual void visit(Rebind_decl*) = 0;
};


// A record declaration.
struct Record_decl : Decl
{
  Record_decl(Symbol const* n, Type const* t, Decl_seq const& m)
    : Decl(n, t), mem_(m)
  { }

  Decl_seq const& members() const { return mem_; }
  void accept(Visitor& v) const { v.visit(this); }
  void accept(Mutator& v)       { v.visit(this); }

  Decl_seq mem_;
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


// A decoder declaration
// A decode declaration  is defined for a type and gives 
// conditions  to determine the next decoder in line.
//
// Stmt* s is a block stmt containing all stmt inside a decoder
// Type* h is the header type 
struct Decode_decl : Decl
{
  Decode_decl(Symbol const* n, Type const* t, Stmt const* s, Type const* h)
    : Decl(n, t), header_(h), body_(s), start_(false)
  { }

  Decode_decl(Symbol const* n, Type const* t, Stmt const* s, Type const* h, bool start)
    : Decl(n, t), header_(h), body_(s), start_(start)
  { }

  Type  const* header() const { return header_; }
  Stmt  const* body()  const { return body_; }
  bool         is_start() const { return start_; }

  void accept(Visitor& v) const { v.visit(this); }

  void set_body(Stmt const* s) { body_ = s; }
  void set_start() { start_ = true; }

  Type const* header_;
  Stmt const* body_;
  bool start_;
};



// A flow table.
struct Table_decl : Decl
{
  // Table kind
  enum Table_kind
  {
    exact_table, 
    wildcard_table,
    prefix_table,
    string_table
  };

  Table_decl(Symbol const* n, Type const* t, int num, Expr_seq const& conds, 
             Decl_seq const& init, Table_kind k)
    : Decl(n, t), num(num), conditions_(conds), body_(init), start_(false), kind_(k)
  { }


  int             number() const     { return num; }
  Expr_seq const& conditions() const { return conditions_; }
  Decl_seq const& body() const { return body_; }
  Table_kind      kind() const { return kind_; }
  bool is_start() const { return start_; }

  void accept(Visitor& v) const { v.visit(this); }

  void set_body(Decl_seq const& d) { body_ = d; }
  void set_start() { start_ = true; }

  int      num;
  Expr_seq conditions_;
  Decl_seq body_;
  bool start_;
  Table_kind kind_;
};


// An entry within a flow table.
//
// FIXME: We should check during compile time that the
// length of the subkey does not exceed the maximum key
// size of the table.
struct Flow_decl : Decl
{
  Flow_decl(Symbol const* n, Type const* t, Expr_seq const& conds, int prio, Stmt const* i)
    : Decl(n, t), prio_(prio), conditions_(conds), instructions_(i)
  { }
  
  int    priority() const { return prio_; }
  Expr_seq const& keys() const { return conditions_; }
  Stmt const*     instructions() const { return instructions_; }

  void accept(Visitor& v) const { v.visit(this); }

  void set_instructions(Stmt const* i) { instructions_ = i; }

  int prio_;
  Expr_seq const conditions_;
  Stmt const* instructions_;
};


// Declaration for extracting a field into a context
struct Extracts_decl : Decl
{
  Extracts_decl(Symbol const* n, Type const* t, Expr const* e)
    : Decl(n, t), field_(e)
  { }

  Expr const* field() const { return field_; }

  void accept(Visitor& v) const { v.visit(this); }

  Expr const* field_;
};


// Extracts a field using the same name as another field
struct Rebind_decl : Decl
{
  Rebind_decl(Symbol const* n, Type const* t, Expr const* e1, Expr const* e2)
    : Decl(n, t), f1(e1), f2(e2)
  { }

  Expr const* field1() const { return f1; }
  Expr const* field2() const { return f2; }

  void accept(Visitor& v) const { v.visit(this); }

  Expr const* f1;
  Expr const* f2;
};


// Declares the name of a port
struct Port_decl : Decl
{
  Port_decl(Symbol const* n, Type const* t)
    : Decl(n, t)
  { }

  void accept(Visitor& v) const { v.visit(this); }

};


// -------------------------------------------------------------------------- //
//                              Queries

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


// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename R>
struct Generic_decl_visitor : Decl::Visitor
{
  Generic_decl_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Record_decl const* d) { r = fn(d); }
  void visit(Variable_decl const* d) { r = fn(d); }
  void visit(Function_decl const* d) { r = fn(d); }
  void visit(Parameter_decl const* d) { r = fn(d); }
  void visit(Module_decl const* d) { r = fn(d); }

  // network declarations
  void visit(Decode_decl const* d) { r = fn(d); }
  void visit(Table_decl const* d) { r = fn(d); }
  void visit(Flow_decl const* d) { r = fn(d); }
  void visit(Port_decl const* d) { r = fn(d); }
  void visit(Extracts_decl const* d) { r = fn(d); }
  void visit(Rebind_decl const* d) { r = fn(d); }

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
  
  void visit(Record_decl const* d) { fn(d); }
  void visit(Variable_decl const* d) { fn(d); }
  void visit(Function_decl const* d) { fn(d); }
  void visit(Parameter_decl const* d) { fn(d); }
  void visit(Module_decl const* d) { fn(d); }

  // network declarations
  void visit(Decode_decl const* d) { fn(d); }
  void visit(Table_decl const* d) { fn(d); }
  void visit(Flow_decl const* d) { fn(d); }
  void visit(Port_decl const* d) { fn(d); }
  void visit(Extracts_decl const* d) { fn(d); }
  void visit(Rebind_decl const* d) { fn(d); }

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
  
  void visit(Record_decl* d) { r = fn(d); }
  void visit(Variable_decl* d) { r = fn(d); }
  void visit(Function_decl* d) { r = fn(d); }
  void visit(Parameter_decl* d) { r = fn(d); }
  void visit(Module_decl* d) { r = fn(d); }

  // network declarations
  void visit(Decode_decl* d) { r = fn(d); }
  void visit(Table_decl* d) { r = fn(d); }
  void visit(Flow_decl* d) { r = fn(d); }
  void visit(Port_decl* d) { r = fn(d); }
  void visit(Extracts_decl* d) { r = fn(d); }
  void visit(Rebind_decl* d) { r = fn(d); }

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
  
  void visit(Record_decl* d) { fn(d); }
  void visit(Variable_decl* d) { fn(d); }
  void visit(Function_decl* d) { fn(d); }
  void visit(Parameter_decl* d) { fn(d); }
  void visit(Module_decl* d) { fn(d); }

  // network declarations
  void visit(Decode_decl* d) { fn(d); }
  void visit(Table_decl* d) { fn(d); }
  void visit(Flow_decl* d) { fn(d); }
  void visit(Port_decl* d) { fn(d); }
  void visit(Extracts_decl* d) { fn(d); }
  void visit(Rebind_decl* d) { fn(d); }

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
