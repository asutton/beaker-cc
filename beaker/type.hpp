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
};


struct Type::Visitor
{
  virtual void visit(Boolean_type const*) = 0;
  virtual void visit(Integer_type const*) = 0;
  virtual void visit(Function_type const*) = 0;
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


// -------------------------------------------------------------------------- //
//                              Type accessors

Type const* get_type_kind();
Type const* get_boolean_type();
Type const* get_integer_type();
Type const* get_function_type(Type_seq const&, Type const*);
Type const* get_function_type(Decl_seq const&, Type const*);


// -------------------------------------------------------------------------- //
//                              Generic visitors

template<typename F, typename R>
struct Generic_type_visitor : Type::Visitor
{
  Generic_type_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Boolean_type const* t) { r = fn(t); }
  void visit(Integer_type const* t) { r = fn(t); }
  void visit(Function_type const* t) { r = fn(t); }

  F fn;
  R r;
};


// A specialization for functions returning void.
template<typename F>
struct Generic_type_visitor<F, void> : Type::Visitor
{
  Generic_type_visitor(F fn)
    : fn(fn)
  { }
  
  void visit(Boolean_type const* t) { fn(t); }
  void visit(Integer_type const* t) { fn(t); }
  void visit(Function_type const* t) { fn(t); }

  F fn;
};


// Dispatch visitor to a void visitor.
template<typename F, typename R = typename std::result_of<F(Boolean_type const*)>::type>
inline typename std::enable_if<std::is_void<R>::value, void>::type
dispatch(Type const* p, F fn)
{
  Generic_type_visitor<F, void> v(fn);
  p->accept(v);
}


// Dispatch to a non-void visitor.
template<typename F, typename R = typename std::result_of<F(Boolean_type const*)>::type>
inline typename std::enable_if<!std::is_void<R>::value, R>::type
dispatch(Type const* p, F fn)
{
  Generic_type_visitor<F, R> v(fn);
  p->accept(v);
  return v.r;
}


// Apply fn to the propositoin p.
template<typename F, typename R = typename std::result_of<F(Boolean_type const*)>::type>
inline R
apply(Type const* p, F fn)
{
  return dispatch(p, fn);
}


#endif

