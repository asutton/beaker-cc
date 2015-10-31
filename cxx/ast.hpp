
#ifndef AST_HPP
#define AST_HPP

#include "nodes.hpp"

namespace cxx
{

// This module defines the high-level framework for representing
// the different kinds of terms in the C++ programming language.
// Specific kinds of terms are found in the name, type, expr,
// stmt, and decl modules.

// -------------------------------------------------------------------------- //
// Declaration specifiers                                          [dcl.spec] //
//
// The declaration specifiers are represented as a set. Note
// that the type specifiers include the cv-qualifiers and
// tag specifiers, but do not include the specifiers for
// fundamental type (e.g., short, long, int, double, etc.).

using Decl_spec = std::uint32_t;
// Storage class specifiers
constexpr Decl_spec register_spec     = 1 << 0;
constexpr Decl_spec static_spec       = 1 << 1;
constexpr Decl_spec thread_local_spec = 1 << 2;
constexpr Decl_spec extern_spec       = 1 << 3;
constexpr Decl_spec mutable_spec      = 1 << 4;
// Function specifiers
constexpr Decl_spec inline_spec       = 1 << 5;
constexpr Decl_spec virtual_spec      = 1 << 6;
constexpr Decl_spec explicit_spec     = 1 << 7;
// Type specifiers
constexpr Decl_spec const_spec        = 1 << 8;
constexpr Decl_spec volatile_spec     = 1 << 9;
constexpr Decl_spec struct_spec       = 1 << 10;
constexpr Decl_spec class_spec        = 1 << 11;
constexpr Decl_spec union_spec        = 1 << 12;
constexpr Decl_spec enum_spec         = 1 << 13;
constexpr Decl_spec typename_spec     = 1 << 14;
constexpr Decl_spec simple_type_spec  = 1 << 15;
// Other specifiers
constexpr Decl_spec friend_spec       = 1 << 16;
constexpr Decl_spec constexpr_spec    = 1 << 17;
constexpr Decl_spec export_spec       = 1 << 18;

// -------------------------------------------------------------------------- //
// Value categories                                              [basic.lval] //
//
// Because not all programs care about correctly categorizing
// expressions, the unknown_cat value can be used as a default
// category. Note that a well-formed program contains only
// categorized expressions.

enum Value_cat {
  unknown_cat, // Unspecified
  lvalue_cat,  // An object or function
  xvalue_cat,  // An rvalue reference to an lvalue
  prvalue_cat, // A temporary or literal
  glvalue_cat, // An lvalue or xvalue
  rvalue_cat   // An xvalue or prvalue
};

// -------------------------------------------------------------------------- //
// C++ terms
//
// There are three main categories of terms in C++: names, types, and
// expressions. Statements and declarations are kinds of expressions.

// Every phrase in the C++ language is characterized as a term.
struct Term : Node { using Node::Node; };

// The name of a declaration.
struct Name : Term { using Term::Term; };

// The base class of all types. Every type has a set of specifiers
// that determine properties of that type. For example, all types
// can have cv-qualified (basic.type.qualifier) versions. Class,
// union, and enum types have their respective tag specifiers.
struct Type : Term {
  Type(Node_kind k)
    : Term(k), ts_() { }
  Type(Node_kind k, Decl_spec s)
    : Term(k), ts_(s) { }

  Decl_spec specifiers() const { return ts_; }

  Decl_spec ts_;
};

// The base class of all expressions. Every expression has a value
// category describes the kinds of value produced by the evaluation
// of the expression.
struct Expr : Term {
  Expr(Node_kind k, Type* t)
    : Term(k), et_(t), vc_(unknown_cat) { }
  Expr(Node_kind k, Type* t, Value_cat c)
    : Term(k), et_(t), vc_(c) { }

  Type* type() const { return et_; }
  Value_cat category() const { return vc_; }

  Type*     et_; // expression type
  Value_cat vc_; // value category
};

// A statement is an expression.
struct Stmt : Expr { using Expr::Expr; };

// A declaration is a statement.
struct Decl : Stmt { 
  Decl(Node_kind k)
    : Stmt(k, nullptr), ds_() { }
  Decl(Node_kind k, Decl_spec d, Type* t)
    : Stmt(k, t), ds_(d) { }

  Decl_spec specifiers() const { return ds_; }

  Decl_spec ds_; // declaration specifiers
};

// Sequences
using Term_seq = Seq<Term>;
using Type_seq = Seq<Type>;
using Expr_seq = Seq<Expr>;
using Stmt_seq = Seq<Stmt>;
using Decl_seq = Seq<Decl>;


// // -------------------------------------------------------------------------- //
// // Constructor

// Token identifier_token(String);

// Expr* make_decimal_literal(int);
// Expr* make_string_literal(String);

// template<typename T, typename... Args> Seq<T>* make_seq(Args...);
// template<typename... Args> Seq<Term>* make_term_seq(Args...);
// template<typename... Args> Seq<Type>* make_type_seq(Args...);
// template<typename... Args> Seq<Expr>* make_expr_seq(Args...);
// template<typename... Args> Seq<Stmt>* make_stmt_seq(Args...);
// template<typename... Args> Seq<Decl>* make_decl_seq(Args...);

// Name* make_id();
// Name* make_id(String);
// Name* make_id(Name);

// template<typename... Args> Name* make_template_id(String, Args...);
// template<typename... Args> Name* make_template_id(Name*, Args...);

// Name* make_qualified_id(String, const char*);
// Name* make_qualified_id(String, String);
// Name* make_qualified_id(String, Name*);
// Name* make_qualified_id(Type, const char*);
// Name* make_qualified_id(Type, String);
// Name* make_qualified_id(Type, Name*);

// template<typename... Args> Name* make_qualified_id(String, Args...);
// template<typename... Args> Name* make_qualified_id(Type, Args...);

// Expr* as_expr(String);
// Expr* as_expr(Name*);

// Type* as_type(String n);
// Type* as_type(Name*);

// Fragment_decl* make_fragment();


// // -------------------------------------------------------------------------- //
// // Printing

// void pretty_print(Printer&, Term*);
// void debug_print(Printer&, Term*);

} // namespace cxx

#endif
