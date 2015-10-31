
#ifndef STMT_HPP
#define STMT_HPP

// This module defines the different kinds of statements in
// the C++ Programming Language. In general, statements define flow
// control and support for grouping or nesting. Unless otherwise
// noted, statements have unspecified type and value.
//
// Note that certain kinds of statements are treated as declarations.
// For example, the labeled-stmt is defined to be a Label_decl because
// all declarations are statements.

#include "ast.hpp"

namespace cxx
{

constexpr Node_kind labeled_stmt   = make_stmt_node(6001); // e;
constexpr Node_kind expr_stmt      = make_stmt_node(6002); // e;
constexpr Node_kind block_stmt     = make_stmt_node(6003); // { stmts* }
constexpr Node_kind if_then_stmt   = make_stmt_node(6004); // if (e) s
constexpr Node_kind if_else_stmt   = make_stmt_node(6005); // if (e) s1 else s2
constexpr Node_kind switch_stmt    = make_stmt_node(6006); // switch (n) l
constexpr Node_kind case_stmt      = make_stmt_node(6007); // case e:
constexpr Node_kind while_stmt     = make_stmt_node(6008); // while (p) s
constexpr Node_kind do_stmt        = make_stmt_node(6009); // do { s } while (p)
constexpr Node_kind for_stmt       = make_stmt_node(6010); // for (e1 ; e2; e3) s4
constexpr Node_kind range_for_stmt = make_stmt_node(6011); // for (d : e) s
constexpr Node_kind goto_stmt      = make_stmt_node(6012); // goto l
constexpr Node_kind return_stmt    = make_stmt_node(6013); // return e;
constexpr Node_kind break_stmt     = make_stmt_node(6014); // break
constexpr Node_kind continue_stmt  = make_stmt_node(6015); // continue
constexpr Node_kind empty_stmt     = make_stmt_node(6016); // ;

// -------------------------------------------------------------------------- //
// Statements

// A labeled statement declares a label and is follwed by a
// statement.
struct Labeled_stmt : Stmt, Kind_of<labeled_stmt> {
  Labeled_stmt(Stmt* s)
    : Stmt(Kind, nullptr, unknown_cat), first(s) { }

  Stmt* stmt() const { return first; }
  
  Stmt* first;
};

// An expression statement has the for 'e;'. The type of this
// statement is the same as that of 'e' and is 'e' is a
// discarded-value expression.
struct Expr_stmt : Stmt, Kind_of<expr_stmt> {
  Expr_stmt(Expr* e)
    : Stmt(Kind, e->type(), e->category()), first(e) { }

  Expr* expr() const { return first; }
  
  Expr* first;
};

// A block statement has the form { s* } where s* is a possibly
// empty sequence of statements.
//
// TODO: the type and value category of a block should be derived
// from its return statements or its last statement.
struct Block_stmt : Stmt, Kind_of<block_stmt> {
  Block_stmt(Type* t, Stmt_seq s)
    : Stmt(Kind, t, unknown_cat), first(s) { }

  Stmt_seq stmts() const { return first; }

  Stmt_seq first;
};

// An if-then statement has the for 'if (e) s' where e is a boolean
// expression and s is a statement that is evalued only when e
// is true.
struct If_then_stmt : Stmt, Kind_of<if_then_stmt> {
  If_then_stmt(Expr* e, Stmt* s)
    : Stmt(Kind, nullptr, unknown_cat), first(e), second(s) { }

  Expr* first;
  Stmt* second;
};

// An if-else statement has the form 'if(e) s1 else s2' where e is
// a boolean expression, s1 is a statement evaluated only when
// e is true, and s2 is a statement evaluated only when e is false.
struct If_else_stmt : Stmt, Kind_of<if_else_stmt> {
  If_else_stmt(Expr* e, Stmt* s1, Stmt* s2)
    : Stmt(Kind, nullptr, unknown_cat), first(e), second(s1), third(s2) { }

  Expr* first;
  Stmt* second;
  Stmt* third;
};

// A switch statement has the form 'switch (e)' s where e is an
// integral expression and 's' is a statement. Note that 's' is
// nearly always a block statement containing if labels (but this
// need not be the case).
struct Switch_stmt : Stmt, Kind_of<switch_stmt> {
  Switch_stmt(Expr* a, Stmt* b)
    : Stmt(Kind, nullptr, unknown_cat), first(a), second(b)
  { }

  Expr* arg() const { return first; }
  Stmt* body() const { return second; }

  Expr* first;
  Stmt* second;
};

// A labeled statement within a switch. Note that the label
// can be a default expression.
struct Case_stmt : Stmt, Kind_of<case_stmt> {
  Case_stmt(Expr* l, Stmt* s)
    : Stmt(Kind, nullptr, unknown_cat), first(l), second(s)
  { }

  Expr* label() const { return first; }
  Stmt* stmt() const { return second; }

  Expr* first;
  Stmt* second;
};

// A while statement.
struct While_stmt : Stmt, Kind_of<while_stmt> {
  Expr* cond() const { return first; }
  Stmt* body() const { return second; }

  Expr* first;
  Stmt* second;
};

struct Do_stmt : Stmt, Kind_of<do_stmt> {
  Expr* cond() const { return first; }
  Stmt* body() const { return second; }

  Expr* first;
  Stmt* second;
};

struct For_stmt : Stmt, Kind_of<for_stmt> {
  Expr* init() const { return first; }
  Expr* cond() const { return second; }
  Expr* update() const { return third; }

  Expr* first;
  Expr* second;
  Expr* third;
};

struct Range_for_stmt : Stmt, Kind_of<range_for_stmt> {
  Decl* var() const { return first; }
  Expr* range() const { return second; }

  Decl* first;
  Expr* second;
};

// A goto statement of the form 'goto l' where 'l' is a
// label declaration.
struct Goto_stmt : Stmt, Kind_of<goto_stmt> {
  Decl* label() const { return first; }

  Decl* first;
};

// A statement of the form 'return e'. 
struct Return_stmt : Stmt, Kind_of<return_stmt> {
  Return_stmt(Expr* e)
    : Stmt(Kind, e->type(), e->category()), first(e) { }

  Expr* value() const { return first; }

  Expr* first;
};

// A statement of the form 'break'. 
struct Break_stmt : Stmt, Kind_of<break_stmt> {
  Break_stmt()
    : Stmt(Kind, nullptr, unknown_cat) { }
};

// A statement of the form 'continue'.
struct Continue_stmt : Stmt, Kind_of<continue_stmt> {
  Continue_stmt()
    : Stmt(Kind, nullptr, unknown_cat) { }
};


struct Empty_stmt : Stmt, Kind_of<empty_stmt> {
  Empty_stmt()
    : Stmt(Kind, nullptr, unknown_cat)
  { }
};

} // namespace cxx

#endif
