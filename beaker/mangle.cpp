// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "mangle.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"

#include <iostream>


// Returns the spelling of d's name.
void
mangle(std::ostream& os, Symbol const* sym)
{
  os << sym->spelling();
}


// -------------------------------------------------------------------------- //
// Mangling of type names


// TODO: In full generality, we actually need to mail
// unresolved types (and dependent expressions), just
// like C++ does.
void
mangle(std::ostream& os, Id_type const* t)
{
  lingo_unreachable();
}


void
mangle(std::ostream& os, Boolean_type const* t)
{
  os << 'b';
}


void
mangle(std::ostream& os, Character_type const* t)
{
  os << 'c';
}


void
mangle(std::ostream& os, Integer_type const* t)
{
  os << 'i';
}


// 'F' p* r
//
// Note that p* expands to the sequence
// of parameter types.
void
mangle(std::ostream& os, Function_type const* t)
{
  os << 'F';
  for (Type const* p : t->parameter_types())
    mangle(os, p);
  mangle(os, t->return_type());
}


// 'A' n t
//
// Note that mangling the type first would make
// it difficult to demangle names.
void
mangle(std::ostream& os, Array_type const* t)
{
  os << 'A';
  os << t->size() << '_';
  mangle(os, t->type());
}


// 'B' t
void
mangle(std::ostream& os, Block_type const* t)
{
  os << 'B';
  mangle(os, t->type());
}


// 'R' t
void
mangle(std::ostream& os, Reference_type const* t)
{
  os << 'R';
  mangle(os, t->type());
}


// 'Tr' n '_'
void
mangle(std::ostream& os, Record_type const* t)
{
  os << "Tr";
  mangle(os, t->declaration());
  os << '_';
}


void
mangle(std::ostream& os, Type const* t)
{
  struct Fn
  {
    std::ostream& os;
    void operator()(Id_type const* t) { return mangle(os, t); }
    void operator()(Boolean_type const* t) { return mangle(os, t); }
    void operator()(Character_type const* t) { return mangle(os, t); }
    void operator()(Integer_type const* t) { return mangle(os, t); }
    void operator()(Function_type const* t) { return mangle(os, t); }
    void operator()(Array_type const* t) { return mangle(os, t); }
    void operator()(Block_type const* t) { return mangle(os, t); }
    void operator()(Reference_type const* t) { return mangle(os, t); }
    void operator()(Record_type const* t) { return mangle(os, t); }
  };
  apply(t, Fn{os});
}


String
mangle(Type const* t)
{
  std::stringstream ss;
  mangle(ss, t);
  return ss.str();
}


// -------------------------------------------------------------------------- //
// Mangling of expressions

void mangle(std::ostream&, Expr const*);


void
mangle(std::ostream& os, Id_expr const* e)
{
  // FIXME: Do I need more mangling information here?
  // Maybe string lenght?
  mangle(os, e->symbol());
}


void
mangle(std::ostream& os, Dot_expr const* e)
{
  mangle(os, e->container());
  os << '_';
  mangle(os, e->member());
}


struct Mangle_expr_fn
{
  std::ostream& os;

  template<typename T>
  void operator()(T const* e) const { lingo_unreachable(); }

  void operator()(Id_expr const* e) const { mangle(os, e); }
  void operator()(Dot_expr const* e) const { mangle(os, e); }

};


void
mangle(std::ostream& os, Expr const* e)
{
  return apply(e, Mangle_expr_fn{os});
}


String
mangle(Expr const* e)
{
  std::stringstream ss;
  mangle(ss, e);
  return ss.str();
}


// -------------------------------------------------------------------------- //
// Mangling of declarations
//
// Always mangle the name and type of each declaration.
//
// FIXME: Every name should (probably) be fully qualified
// by its context. We need to worry about that when we
// have modules or namespaces, or if we allow nested types.



void
mangle_scope(std::ostream& os, Decl const* d)
{
  if (Module_decl const* m = d->module())
    if (Expr* e = m->module()) {
      mangle(os, e);
      os << '_';
    }
}


void
mangle(std::ostream& os, Variable_decl const* d)
{
  mangle_scope(os, d);
  mangle(os, d->name());
  os << '_';
  mangle(os, d->type());
}


void
mangle(std::ostream& os, Function_decl const* d)
{
  mangle_scope(os, d);
  mangle(os, d->name());
  os << '_';
  mangle(os, d->type());
}


void
mangle(std::ostream& os, Parameter_decl const* d)
{
  // I should never get here.
}


// The name of the type.
void
mangle(std::ostream& os, Record_decl const* d)
{
  mangle(os, d->name());
}


void
mangle(std::ostream& os, Field_decl const* d)
{
  // Should never get here...
}


void
mangle(std::ostream& os, Method_decl const* d)
{
  mangle_scope(os, d);
  mangle(os, d->context());
  os << '_';
  mangle(os, d->name());
  os << '_';
  mangle(os, d->type());
}


void
mangle(std::ostream& os, Module_decl const* d)
{
  // Should never get here...
}


void
mangle(std::ostream& os, Decl const* d)
{

  struct Fn
  {
    std::ostream& os;
    void operator()(Variable_decl const* d) { mangle(os, d); }
    void operator()(Function_decl const* d) { mangle(os, d); }
    void operator()(Parameter_decl const* d) { mangle(os, d); }
    void operator()(Record_decl const* d) { mangle(os, d); }
    void operator()(Field_decl const* d) { mangle(os, d); }
    void operator()(Method_decl const* d) { mangle(os, d); }
    void operator()(Module_decl const* d) { mangle(os, d); }
  };
  apply(d, Fn{os});
}


String
mangle(Decl const* d)
{
  std::stringstream ss;
  mangle(ss, d);
  return ss.str();
}
