// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_TYPE_HPP
#define BEAKER_TYPE_HPP

#include "prelude.hpp"


// The Type class represents the set of all types in the
// language.
//
//    t ::= bool                -- boolean type
//          int                 -- integer type
//          (t1, ..., tn) -> t  -- function types
//          ref t               -- reference types
//
// Note that types are not mutable. Once created, a type
// cannot be changed. The reason for this is that we
// internally canonicalize (most) types when they are
// created.
//
// The "type" type (or kind) denotes the type user-defined
// types. Although it describes the higher-level kind
// system, we include it with the type system for
// convenience.
struct Type
{
  struct Visitor;

  virtual ~Type() { }

  virtual void accept(Visitor&) const = 0;

  virtual Type const* ref() const;
  virtual Type const* nonref() const;
};


struct Type::Visitor
{
  virtual void visit(Id_type const*) = 0;
  virtual void visit(Boolean_type const*) = 0;
  virtual void visit(Integer_type const*) = 0;
  virtual void visit(Function_type const*) = 0;
  virtual void visit(Reference_type const*) = 0;
  virtual void visit(Record_type const*) = 0;
};


// A type named by an identifier. These are Essentially
// placeholders to be determined during initialization.
struct Id_type : Type
{
  Id_type(Symbol const* s)
    : sym_(s)
  { }

  void accept(Visitor& v) const { v.visit(this); };

  Symbol const* symbol() const { return sym_; }

  Symbol const* sym_;
};


// The type bool.
struct Boolean_type : Type
{
  void accept(Visitor& v) const { v.visit(this); };
};


// The type int.
struct Integer_type : Type
{
  void accept(Visitor& v) const { v.visit(this); };
};


// Represents function types (t1, ..., tn) -> t.
struct Function_type : Type
{
  Function_type(Type_seq const& t, Type const* r)
    : first(t), second(r)
  { }

  void accept(Visitor& v) const { v.visit(this); };

  Type_seq const& parameter_types() const { return first; }
  Type const*     return_type() const     { return second; }

  Type_seq    first;
  Type const* second;
};


// The type of an expression that refers to an object.
struct Reference_type : Type
{
  Reference_type(Type const* t)
    : first(t)
  { }

  void accept(Visitor& v) const { v.visit(this); };

  virtual Type const* ref() const;
  virtual Type const* nonref() const;

  Type const* type() const { return first; }


  Type const* first;
};


// A record type is the type introduced by a
// record declaration.
struct Record_type : Type
{
  Record_type(Decl* d)
    : decl_(d)
  { }

  void accept(Visitor& v) const { v.visit(this); };

  Record_decl* declaration() const;

  Decl* decl_;
};


// -------------------------------------------------------------------------- //
//                              Type accessors

Type const* get_type_kind();
Type const* get_id_type(Symbol const*);
Type const* get_boolean_type();
Type const* get_integer_type();
Type const* get_function_type(Type_seq const&, Type const*);
Type const* get_function_type(Decl_seq const&, Type const*);
Type const* get_reference_type(Type const*);
Type const* get_record_type(Record_decl*);


// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename T>
struct Generic_type_visitor : Type::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_type_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }

  void visit(Id_type const* t) { this->invoke(t); }
  void visit(Boolean_type const* t) { this->invoke(t); }
  void visit(Integer_type const* t) { this->invoke(t); }
  void visit(Function_type const* t) { this->invoke(t); }
  void visit(Reference_type const* t) { this->invoke(t); }
  void visit(Record_type const* t) { this->invoke(t); }
};


template<typename F, typename T = typename std::result_of<F(Boolean_type const*)>::type>
inline T
apply(Type const* t, F fn)
{
  Generic_type_visitor<F, T> v(fn);
  return accept(t, v);
}



#endif
