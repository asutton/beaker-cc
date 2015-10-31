
#ifndef EXPR_HPP
#define EXPR_HPP

// This module defines the expressions in the C++ Programming 
// Language. Note that certain kinds of postfix, unary, and
// binary expressions can be used with overloadable operators.
// In those cases, there is no distinct expression for that
// form; the expression is formed as a Unary_expr or Binary_expr
// with an appropriate Unary_kind or Binary_kind.

#include "ast.hpp"

namespace cxx
{

// Literal primary expressions
constexpr Node_kind int_expr              = make_term_node(2001);
constexpr Node_kind char_expr             = make_term_node(2002);
constexpr Node_kind float_expr            = make_term_node(2003);
constexpr Node_kind string_expr           = make_term_node(2004);
constexpr Node_kind bool_expr             = make_term_node(2005);
constexpr Node_kind pointer_expr          = make_term_node(2006);
// Other primary expressions
constexpr Node_kind this_expr             = make_term_node(5100); // this
constexpr Node_kind default_expr          = make_term_node(5101); // default
constexpr Node_kind id_expr               = make_term_node(5102); // n
constexpr Node_kind lambda_expr           = make_term_node(5103); // [c]d{e}
constexpr Node_kind init_expr             = make_term_node(5103); // {e1, ..., en}
// Postfix expressions
constexpr Node_kind call_expr             = make_term_node(5201); // e(e1, ..., en)
constexpr Node_kind construct_expr        = make_term_node(5202); // T(e1, ..., en)
constexpr Node_kind dot_expr              = make_term_node(5203); // e1.e2
constexpr Node_kind dynamic_cast_expr     = make_term_node(5207); // dynamic_cast<T>(e)
constexpr Node_kind static_cast_expr      = make_term_node(5208); // static_cast<T>(e)
constexpr Node_kind reinterpret_cast_expr = make_term_node(5209); // reinterpret_cast<T>(e)
constexpr Node_kind const_cast_expr       = make_term_node(5210); // const_cast<T>(e)
constexpr Node_kind expr_typeid_expr      = make_term_node(5211); // typeid(e)
constexpr Node_kind type_typeid_expr      = make_term_node(5211); // typeid(T)
// Unary expressions
constexpr Node_kind expr_sizeof_expr      = make_term_node(5214); // sizeof e
constexpr Node_kind type_sizeof_expr      = make_term_node(5215); // sizeof(T)
constexpr Node_kind sizeof_pack_expr      = make_term_node(5216); // sizeof... (e)
constexpr Node_kind alignof_expr          = make_term_node(5217); // alignof(T)
constexpr Node_kind noexcept_expr         = make_term_node(5218); // noexcept(e)
constexpr Node_kind new_expr              = make_term_node(5219); // new T(e)
constexpr Node_kind delete_expr           = make_term_node(5220); // delete (e)
// Misc. expressions
constexpr Node_kind cast_expr             = make_term_node(5230); // (T)e
constexpr Node_kind dot_star_expr         = make_term_node(5231); // e1.*e2
constexpr Node_kind cond_expr             = make_term_node(5232); // e1 ? e2 : e3
constexpr Node_kind unary_expr            = make_term_node(5240); // op e
constexpr Node_kind binary_expr           = make_term_node(5241); // e1 op e2

// Enumerates the overloadable unary operator kinds.
enum Unary_kind {
  // Object operators
  deref_op,   // *e
  addr_op,    // &e
  // Arithmetic operators
  pos_op,     // +e
  neg_op,     // -e
  // Bitwise operators
  bnot_op,    // ~e
  // Assignment operators
  preinc_op,  // ++e
  predec_op,  // --e
  postinc_op, // e++
  postdec_op, // e--
  // Logical operators
  not_op     // !e
};

// Enumerates the kinds of overloadable binary operators.
enum Binary_kind {
  // Object operators
  arrow_op,       // e1->e2
  arrow_star_op,  // e1->*e2
  subscript_op,   // e1[e2]
  // Arithmetic operators
  mul_op,         // e1 * e2
  div_op,         // e1 / e2
  mod_op,         // e1 % e2
  add_op,         // e1 + e2
  sub_op,         // e1 - e2
  // Bitwise expressions
  lsh_op,         // e1 << e2
  rsh_op,         // e1 >> e2
  band_op,        // e1 & e2
  bxor_op,        // e1 ^ e2
  bor_op,         // e1 | e2
  // Relational expressions
  lt_op,          // e1 < e2
  gt_op,          // e1 > e2
  le_op,          // e1 <= e2
  ge_op,          // e1 >= e2
  eq_op,          // e1 == e2
  ne_op,          // e1 != e2
  // Locigal operators
  and_op,         // e1 && e2
  or_op,          // e1 || e2
  // Assignment operators
  assign_op,      // e1 = e2
  mul_assign_op,  // e1 *= e2
  div_assign_op,  // e1 /= e2
  mod_assign_op,  // e1 %= e2
  add_assign_op,  // e1 += e2
  sub_assign_op,  // e1 *= e2
  lsh_assign_op,  // e1 <<= e2
  rsh_assign_op,  // e1 >>= e2
  band_assign_op, // e1 &= e2
  bxor_assign_op, // e1 ^= e2
  bor_assign_op,  // e1 |= e2
  // Comma operators
  comma_op        // e1, ..., en
};


// -------------------------------------------------------------------------- //
// Literals                                                     [lex.literal] //
//
// Each kind of literal is represented as a distinct kind of expression.



// The literal class is a helper class for the definition of
// literal expressions.
template<typename T, Node_kind K, Value_cat C = prvalue_cat>
  struct Literal_expr : Expr, Kind_of<K> {
    Literal_expr(Type* t, const T& x)
      : Expr(K, t, C), first(x) { }

    const T& value() const { return first; }

    T first;
  };

// An integer literal expression.
struct Int_expr : Literal_expr<Integer, int_expr, prvalue_cat> {
  using Literal_expr<Integer, int_expr, prvalue_cat>::Literal_expr;
};

// A character literal expression.
//
// TODO: Make a better character representation.
struct Char_expr : Literal_expr<char, char_expr, prvalue_cat> {
  using Literal_expr<char, char_expr, prvalue_cat>::Literal_expr;
};

// A floating point literal expression.
//
// TODO: Make a better floating point representation.
struct Float_expr : Literal_expr<double, float_expr, prvalue_cat> {
  using Literal_expr<double, float_expr, prvalue_cat>::Literal_expr;
};

// A string literal expression.
//
// TODO: Make a better string representation.
struct String_expr : Literal_expr<String, string_expr, lvalue_cat> {
  using Literal_expr<String, string_expr, lvalue_cat>::Literal_expr;
};

// A boolean literal expression.
struct Bool_expr : Literal_expr<bool, bool_expr, lvalue_cat> {
  using Literal_expr<bool, bool_expr, lvalue_cat>::Literal_expr;
};

// A pointer literal expression.
struct Pointer_expr : Expr, Kind_of<pointer_expr> {
  Pointer_expr(Type* t)
    : Expr(pointer_expr, t, prvalue_cat) { }
};

// An id-expression is a name that refers to a declaration.
// Note that it unresolved contexts, the referent declaration
// may be an overload set or a member of an unknown specialization.
struct Id_expr : Expr, Kind_of<id_expr> {
  Id_expr(Type* t, Value_cat c, Name* n, Decl* d)
    : Expr(id_expr, t, c), first(n), second(d) { }

  Name* name() const { return first; }
  Decl* decl() const { return second; }

  Name* first;
  Decl* second;
};

// The 'this' expression.
//
// TODO: What is the value category of this? 
struct This_expr : Expr, Kind_of<this_expr> {
  This_expr(Type* t)
    : Expr(pointer_expr, t, lvalue_cat) { }
};

// The 'default' expression.  Note that 'default' is not
// generally treated as an expression, but it is nice to
// model it as such.
struct Default_expr : Expr, Kind_of<this_expr> {
  Default_expr(Type* t)
    : Expr(pointer_expr, t, unknown_cat) { }
};

// A braced initializer list has the form '{e1, ..., en}'.
// Note that this expression shall have unknown type.
struct Init_expr : Expr, Kind_of<init_expr> {
  Init_expr(Type* t, Expr_seq e)
    : Expr(init_expr, t, unknown_cat), first(e) { }

  Expr_seq elems() const { return first; }

  Expr_seq first;
};

// A lambda expression.
//
// TODO: Implement me. Note that captures are probably some kind
// of unary expression (value-capture, reference-capture, etc.).
struct Lambda_expr : Expr, Kind_of<lambda_expr> {
  Term* first;
  Decl* second;
  Stmt* third;
};

// A function call, possibly to a set of overloaded functions.
// Note that the call target is a declaration after overload
// resolution is performed.
//
// See 5.2.2p10 for the value category of this expression.
struct Call_expr : Expr, Kind_of<call_expr> {
  Call_expr(Type* t, Value_cat c, Expr* f, Expr_seq a, Name* n)
    : Expr(Kind, t, c), first(f), second(a), third(n) 
  { }

  Expr* fn() const { return first; }
  Expr_seq args() const { return second; }
  Name* fn_name() const { return third; }

  Expr* first;
  Expr_seq second;
  Name* third;
};

// An explicit conversion of the form 'T(e1, ..., en)'.
struct Construct_expr : Expr, Kind_of<construct_expr> {
  Construct_expr(Type* t, Expr_seq a)
    : Expr(Kind, t, prvalue_cat), first(t), second(a) { }

  Type* result() const { return first; }
  Expr_seq args() const { return second; }

  Type* first;
  Expr_seq second;
};

// A member access expression of the form 'e1.e2'.
struct Dot_expr : Expr, Kind_of<dot_expr> {
  Dot_expr(Type* t, Value_cat c, Expr* e1, Expr* e2)
    : Expr(Kind, t, c), first(e1), second(e2)
  { }

  Expr* object() const { return first; }
  Expr* member() const { return second; }

  Expr* first;
  Expr* second;
};

// A pointer-to-member access expression of the form 'e1.*e2'.
struct Dot_star_expr : Expr, Kind_of<dot_star_expr> {
  Expr* first;
  Expr* second;
};

// A dynamic cast expression of the form 'dynamic_cast<T>(e)'
// FIXME: change that unknown_cat
struct Dynamic_cast_expr : Expr, Kind_of<dynamic_cast_expr> {
  Dynamic_cast_expr(Type* t, Expr* e)
    : Expr(Kind, t, unknown_cat), first(t), second(e)
  { }

  Type* cast_type() const { return first; }
  Expr* object() const { return second; }

  Type* first;
  Expr* second;
};

// A static cast expression.
struct Static_cast_expr : Expr, Kind_of<static_cast_expr> {
  Type* first;
  Expr* second;
};

// A reinterpret cast expression.
struct Reinterpret_cast_expr : Expr, Kind_of<reinterpret_cast_expr> {
  Reinterpret_cast_expr(Type* t, Expr* e)
    : Expr(Kind, t, unknown_cat), first(t), second(e)
  { }

  Type* cast_type() const { return first; }
  Expr* object() const { return second; }

  Type* first;
  Expr* second;
};

// A const cast expression.
struct Const_cast_expr : Expr, Kind_of<const_cast_expr> {
  Type* first;
  Expr* second;
};

// An explicit cast expression of the form '(T)e'.
struct Cast_expr : Expr, Kind_of<cast_expr> {
  Type* first;
  Expr* second;
};

// A typeid expression of the form 'typeid(e)'.
struct Expr_typeid_expr : Expr, Kind_of<expr_typeid_expr> {
  Expr_typeid_expr(Type* t, Expr* e)
    : Expr(Kind, t, prvalue_cat), first(e) { }

  Expr* arg() const { return first; }

  Expr* first;
};

// A typeid expression of the form 'typeid(T)'.
struct Type_typeid_expr : Expr, Kind_of<type_typeid_expr> {
  Type_typeid_expr(Type* t1, Type* t2)
    : Expr(Kind, t1, prvalue_cat), first(t2) { }

  Type* arg() const { return first; }

  Type* first;
};

// A sizeof expression of the form 'sizeof e'.
struct Expr_sizeof_expr : Expr, Kind_of<expr_sizeof_expr> {
  Expr_sizeof_expr(Type* t, Expr* e)
    : Expr(Kind, t, prvalue_cat), first(e) { }

  Expr* arg() const { return first; }

  Expr* first;
};

// A sizeof expression of the form 'sizeof(T)'.
struct Type_sizeof_expr : Expr, Kind_of<type_sizeof_expr> {
  Type_sizeof_expr(Type* t1, Type* t2)
    : Expr(Kind, t1, prvalue_cat), first(t2) { }

  Type* arg() const { return first; }

  Type* first;
};

// An alignof expression of the form 'alignof(T)'.
struct Alignof_expr : Expr, Kind_of<alignof_expr> {
  Alignof_expr(Type* t1, Type* t2)
    : Expr(Kind, t1, prvalue_cat), first(t2) { }

  Type* arg() const { return first; }

  Type* first;
};

// A noexcept expression of the form 'noexcept(e)'
struct Noexcept_expr : Expr, Kind_of<noexcept_expr> {
  Noexcept_expr(Type* t, Expr* e)
    : Expr(Kind, t, prvalue_cat), first(e) { }

  Expr* arg() const { return first; }

  Expr* first;
};

// A conditional expression of the form 'e1 ? e2 : e3'.
struct Cond_expr : Expr, Kind_of<cond_expr> {
  Cond_expr(Type* t, Value_cat c, Expr* e1, Expr* e2, Expr* e3)
    : Expr(Kind, t, c), first(e1), second(e2), third(e3) { }

  Expr* cond() const { return first; }
  Expr* then() const { return second; }
  Expr* other() const { return third; }

  Expr* first;
  Expr* second;
  Expr* third;
};

// A unary operator expression of the form 'op e'.
//
//
// Note that the called function is an intrinsic member of
// the expression, but not a true operand.
struct Unary_expr : Expr, Kind_of<unary_expr> {
  Unary_expr(Type* t, Value_cat c, Expr* f, Unary_kind k, Expr* e)
    : Expr(Kind, t, c), fn_(f), first(k), second(e) { }

  Unary_kind op() const { return first; }
  Expr* arg() const { return second; }
  Expr* fn() const {return fn_; }

  Expr*      fn_;    // The target function
  Unary_kind first;
  Expr*      second;
};

// A binary operator expression of the form 'e1 op e2'.
//
// Note that the called function is an intrinsic member of
// the expression, but not a true operand.
struct Binary_expr : Expr, Kind_of<binary_expr> {
  Binary_expr(Type* t, Value_cat c, Expr* f, Binary_kind k, Expr* l, Expr* r)
    : Expr(Kind, t, c), fn_(f), first(k), second(l), third(r) { }

  Binary_kind op() const { return first; }
  Expr* left() const { return second; }
  Expr* right() const { return third; }
  Expr* fn() const {return fn_; }

  Expr*       fn_;    // The target function
  Binary_kind first;
  Expr*       second;
  Expr*       third;
};


} // namespace cxx

#endif
