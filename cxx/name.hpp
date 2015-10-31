
#ifndef NAME_HPP
#define NAME_HPP

#include "ast.hpp"

namespace cxx
{

// This module defines the kinds of names that are used to
// declare functions, variables, and types. These terms also 
// correspond to the kinds of terms that can are id-expressions.

constexpr Node_kind basic_id              = make_name_node(1000); // identifier
constexpr Node_kind constructor_id        = make_name_node(1001); // T
constexpr Node_kind destructor_id         = make_name_node(1002); // ~T
constexpr Node_kind operator_id           = make_name_node(1003); // operator x
constexpr Node_kind literal_id            = make_name_node(1004); // operator "" x
constexpr Node_kind conversion_id         = make_name_node(1005); // operator T
constexpr Node_kind qualified_id          = make_name_node(1006); // n1::n2:: ... :: n
constexpr Node_kind template_id           = make_name_node(1007); // X<A1, A2, ..., An>
constexpr Node_kind intrinsic_id          = make_name_node(1008); // missing name

// -------------------------------------------------------------------------- //
// Names

// A basic-id names a declared entity (variable, type, etc). The
// name is comprised of a single identifier token.
struct Basic_id : Name, Kind_of<basic_id> {
  Basic_id(String n)
    : Name(Kind), first(n) { }

  String name() const { return first; }

  String first;
};

// The name of a constructor refers to its type.
struct Constructor_id : Name, Kind_of<constructor_id> {
  Constructor_id(Type* t)
    : Name(Kind), first(t) { }

  Type* type() const { return first; }
  
  Type* first;
};

// The name of a destructor refers to its type. It
// is textually prefixed by a '~'.
struct Destructor_id : Name, Kind_of<destructor_id> {
  Destructor_id(Type* t)
    : Name(Kind), first(t) { }
  
  Type* type() const { return first; }

  Type* first;
};

// The name of an operator function. The specific operator
// is represented as a string; it must be one of the overloadable
// operators.
struct Operator_id : Name, Kind_of<operator_id> {
  Operator_id(String s)
    : Name(Kind), first(s) { }
  
  String name() const { return first; }

  String first;
};

// The name of a conversion operator; it names a type.
struct Conversion_id : Name, Kind_of<conversion_id> {
  Conversion_id(Type* t)
    : Name(Kind), first(t) { }
  
  Type* type() const { return first; }

  Type* first;
};

// The name of a user-defined literal operator of
// the form 'n' when 'n' is adjacent to a literal.
struct Literal_id : Name, Kind_of<literal_id> {
  Literal_id(String s)
    : Name(Kind), first(s) { }
  
  String suffix() const { return first; }

  String first;
};

// A qualified name 's::n' is represented by a pair 's' a type
// representing a scope, and a name 'n'. Note that a namespace
// defines a type. Qualified names can be nested; the name
// 'a::b::c' is a qualified name 'a::q' where 'q' is the qualified
// name 'b::c'.
struct Qualified_id : Name, Kind_of<qualified_id> {
  Qualified_id(Type* t, Name* n)
    : Name(Kind), first(t), second(n) { }
  
  Type* scope() const { return first; }
  Name* name() const { return second; }

  Type* first;
  Name* second;
};

// The name of a template specialization. A template-id
// refers to a declaration and is qualified by a sequence
// of template arguments. The declaration may be a set of
// overloaded functions or operators (including literal
// operators).
struct Template_id : Name, Kind_of<template_id> {
  Template_id(Decl* d, Term_seq* a)
    : Name(Kind), first(d), second(a) { }

  Decl* tmpl() const { return first; }
  Term_seq* args() const { return second; }

  Decl* first;
  Term_seq* second;
};

// An intrinsic name is an internally defined id that
// is not rendered as part of the grammar. Examples include
// the name of the anonymous namespace, the names of
// unnamed (template) parameters, and the names of lambda
// closure types.
struct Intrinsic_id : Name, Kind_of<intrinsic_id> {
  Intrinsic_id(String s)
    : Name(Kind) { }

  String name() const { return first; }

  String first;
};

} // namespace cxx

#endif
