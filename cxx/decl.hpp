
#ifndef DECL_HPP
#define DECL_HPP

#include "ast.hpp"

namespace cxx
{

// Declarations
constexpr Node_kind label_decl            = make_decl_node(1300); // l:
constexpr Node_kind variable_decl         = make_decl_node(1301); // T x = e;
constexpr Node_kind function_decl         = make_decl_node(1302); // T f(P1, P2, ..., Pn)
constexpr Node_kind type_decl             = make_decl_node(1303); // [class|union|enum] n
constexpr Node_kind namespace_decl        = make_decl_node(1304); // namespace N
constexpr Node_kind alias_decl            = make_decl_node(1305); // [using|namespace] N = X
constexpr Node_kind using_decl            = make_decl_node(1306); // using [namespace] N;
constexpr Node_kind access_decl           = make_decl_node(1307); // member access label
constexpr Node_kind bitfield_decl         = make_decl_node(1308); // bit field
constexpr Node_kind enumerator_decl       = make_decl_node(1309); // enumerator
constexpr Node_kind template_decl         = make_decl_node(1310); // template<...> decl
constexpr Node_kind instantiation_decl    = make_decl_node(1311); // template decl;
constexpr Node_kind specialization_decl   = make_decl_node(1312); // template<...> decl;
// Special declarations
constexpr Node_kind translation_decl      = make_decl_node(1340); // translation unit
constexpr Node_kind parameter_decl        = make_decl_node(1342); // parameter declarations
constexpr Node_kind foreign_decl          = make_decl_node(1343); // extern declarations
constexpr Node_kind foreign_c_decl        = make_decl_node(1344); // extern "C" declarations
constexpr Node_kind foreign_cpp_decl      = make_decl_node(1345); // extern "C++" declarations


// -------------------------------------------------------------------------- //
// Declarations

// A lable introduced by a label statement.
struct Label_decl : Decl, Kind_of<label_decl> {
  Label_decl(Name* n)
    : Decl(label_decl), first(n) 
  { }

  Name* name() const { return first; }

  Name* first;
};

// A variable declaration.
struct Variable_decl : Decl, Kind_of<variable_decl> {
  Variable_decl(Decl_spec d, Name* n, Type* t, Expr* e)
    : Decl(variable_decl, d, t), first(n), second(t), third(e) 
  { }

  Name* name() const { return first; }
  Type* type() const { return second; }
  Expr* init() const { return third; }

  Name* first;
  Type* second;
  Expr* third;
};

// A function declaration.
struct Function_decl : Decl, Kind_of<function_decl> {
  Function_decl(Decl_spec d, Name* n, Decl_seq p, Type* t, Expr* b)
    : Decl(function_decl, d, t), name_(n), parms_(p), result(t), body_(b)
  { }

  Name* name() const { return name_; }
  Decl_seq parms() const { return parms_; }
  Type* ret_type() const { return result; }
  Expr* body() const { return body_; }

  Name* name_;
  Decl_seq parms_;
  Type* result;
  Expr* body_;
};

// A type declaration introduces a new type.
struct Type_decl : Decl, Kind_of<type_decl> {
  Type_decl(Type* t)
    : Decl(type_decl), type_(t)
  { }

  Type* type() const { return type_; }

  Type* type_;
};

// // A namespace declaration.
// struct Namespace_decl : Decl {
//   Namespace_decl(Decl_spec d, Type* t)
//     : Decl(namespace_decl, d, t), ns(t)
//   { }

//   Type* ns;
// };

// // An alias to a type. 
// // Note that a namespace alias is a special case.
// struct Alias_decl : Decl {
//   Alias_decl(Name*, Type*);
//   Name* name;
//   Type* value;
// };

// // A declaration of the form "using N" where N is
// // a qualified name.
// struct Using_decl : Decl {
//   Using_decl(Name*);
//   Name* name;
// };

// // An access label declaration.
// struct Access_decl : Decl {
//   Access_decl(Spec);
// };

// // A bit field declaration.
// struct Bitfield_decl : Decl {
//   Bitfield_decl(Spec, Name*, Type*, Expr*);
//   Name* name;
//   Type* type;
//   Expr* bits;
// };

// An enumerator declaration.
struct Enumerator_decl : Decl, Kind_of<enumerator_decl> {
  Enumerator_decl(Name* n, Expr* e)
    : Decl(enumerator_decl), n(n), val(e)
  { }

  Name* name() const { return n; }
  Expr* value() const { return val; }

  Name* n;
  Expr* val;
};

// // A template declaration.
// struct Template_decl : Decl { 
//   Template_decl(Spec, Seq<Decl>*, Decl*);
//   Seq<Decl>* parms;
//   Decl* decl;
// };

// // An explicit instantiation. 
// struct Instantiation_decl : Decl {
//   Instantiation_decl(Spec, Decl*);
//   Decl* decl;
// };

// // An explicit or partial specialization.
// struct Specialization_decl : Decl {
//   Specialization_decl(Spec, Seq<Decl>*, Decl*);
//   Seq<Decl>* parms;
//   Decl* decl;
// };

// // A translation unit, comprised of a sequence of expresssions. 
// // Note that a declaration is an expression. 
// struct Translation_decl : Decl {
//   Translation_decl(Seq<Stmt>*);
//   Seq<Stmt>* stmts;
// };

// // A fragment of program, comprised of a sequence of expressions.
// // Fragments are helpful when constructing programs by part, as it
// // provides a non-C++, semantics-free container for terms.
// //
// // Fragment declarations are also used to impose extern linkage
// // on a sequence of nested declarations (e.g., extern "C" {...}).
// struct Fragment_decl : Decl {
//   Fragment_decl(Seq<Stmt>*);
//   Seq<Stmt>* stmts;
// };

// A parameter declaration is a declaration in a parameter list. 
// Parameters may have default arguments.
//
// Note that this is distinct, for example from a variable
// declaration, which (may) have an initializer. The default
// argument is given distinct semantics.
//
// The nested declaration in a parameter declaration shall not
// have an initializer or definition.
//
// FIXME: Consider embedding depth and offset information
// with parameters. Or maybe provide that as a wrapper 
// facility.
struct Parameter_decl : Decl, Kind_of<parameter_decl> {
  Parameter_decl(Decl_spec d, Name* n, Type* t)
    : Decl(parameter_decl, d, t), n(n)
  { }

  Name* name() const { return n; }

  Name* n;
};

// An foreign linkage specification declaration.
// i.e. extern type function_decl()
struct Foreign_decl : Decl, Kind_of<foreign_decl> {
  Foreign_decl(Decl* d)
    : Decl(foreign_decl), decl(d)
  { };

  Decl* decl;
};


// An foreign linkage specification declaration.
// i.e. extern "C" type function_decl()
struct Foreign_c_decl : Decl, Kind_of<foreign_c_decl> {
  Foreign_c_decl(Decl* d)
    : Decl(foreign_c_decl), decl(d)
  { };
  
  Decl* decl;
};


// An foreign linkage specification declaration.
// i.e. extern "C++" type function_decl()
struct Foreign_cpp_decl : Decl, Kind_of<foreign_cpp_decl> {
  Foreign_cpp_decl(Decl* d)
    : Decl(foreign_cpp_decl), d(d)
  { };

  Decl const* decl() const { return d; }
  
  Decl* d;
};

} // namespace cxx

#endif
