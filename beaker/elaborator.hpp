// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_ELABORATOR_HPP
#define BEAKER_ELABORATOR_HPP

// The elaborator is responsible for a number of static
// analyses. In particular, it resolves identifiers and
// types expressions.
//
// FIXME: The type checking here is fundamentally broken.
// Instead of throwing exceptions, we should be documenting
// errors and continuing elaboration. There may be some
// cases where elaboration must stop.

#include <beaker/prelude.hpp>
#include <beaker/location.hpp>
#include <beaker/scope.hpp>

#include <unordered_set>


// Track defined declarations.
using Decl_set = std::unordered_set<Decl*>;


// Track recursive definitions of records.
using Decl_stack = std::vector<Decl*>;


// The elaborator is responsible for the annotation of
// an AST with type and other information.
class Elaborator
{
  struct Scope_sentinel;
  struct Defining_sentinel;

public:
  Elaborator(Location_map&, Symbol_table&);

  Type const* elaborate_type(Type const*);
  Type const* elaborate_def(Type const*);
  Type const* elaborate(Type const*);
  Type const* elaborate(Id_type const*);
  Type const* elaborate(Boolean_type const*);
  Type const* elaborate(Character_type const*);
  Type const* elaborate(Integer_type const*);
  Type const* elaborate(Float_type const*);
  Type const* elaborate(Double_type const*);
  Type const* elaborate(Function_type const*);
  Type const* elaborate(Array_type const*);
  Type const* elaborate(Block_type const*);
  Type const* elaborate(Reference_type const*);
  Type const* elaborate(Record_type const*);

  Expr* elaborate(Expr*);
  Expr* elaborate(Literal_expr*);
  Expr* elaborate(Id_expr*);
  Expr* elaborate(Decl_expr*);
  Expr* elaborate(Add_expr* e);
  Expr* elaborate(Sub_expr* e);
  Expr* elaborate(Mul_expr* e);
  Expr* elaborate(Div_expr* e);
  Expr* elaborate(Rem_expr* e);
  Expr* elaborate(Neg_expr* e);
  Expr* elaborate(Pos_expr* e);
  Expr* elaborate(Eq_expr* e);
  Expr* elaborate(Ne_expr* e);
  Expr* elaborate(Lt_expr* e);
  Expr* elaborate(Gt_expr* e);
  Expr* elaborate(Le_expr* e);
  Expr* elaborate(Ge_expr* e);
  Expr* elaborate(And_expr* e);
  Expr* elaborate(Or_expr* e);
  Expr* elaborate(Not_expr* e);
  Expr* elaborate(Call_expr* e);
  Expr* elaborate(Dot_expr* e);
  Expr* elaborate(Field_expr* e);
  Expr* elaborate(Method_expr* e);
  Expr* elaborate(Index_expr* e);
  Expr* elaborate(Value_conv* e);
  Expr* elaborate(Block_conv* e);
  Expr* elaborate(Default_init* e);
  Expr* elaborate(Copy_init* e);
  Expr* elaborate(Reference_init* e);

  Decl* elaborate(Decl*);
  Decl* elaborate(Variable_decl*);
  Decl* elaborate(Function_decl*);
  Decl* elaborate(Parameter_decl*);
  Decl* elaborate(Record_decl*);
  Decl* elaborate(Field_decl*);
  Decl* elaborate(Method_decl*);
  Decl* elaborate(Module_decl*);

  // Support for two-phase elaboration.
  Decl* elaborate_decl(Decl*);
  Decl* elaborate_decl(Variable_decl*);
  Decl* elaborate_decl(Function_decl*);
  Decl* elaborate_decl(Parameter_decl*);
  Decl* elaborate_decl(Record_decl*);
  Decl* elaborate_decl(Field_decl*);
  Decl* elaborate_decl(Method_decl*);
  Decl* elaborate_decl(Module_decl*);

  Decl* elaborate_def(Decl*);
  Decl* elaborate_def(Variable_decl*);
  Decl* elaborate_def(Function_decl*);
  Decl* elaborate_def(Parameter_decl*);
  Decl* elaborate_def(Record_decl*);
  Decl* elaborate_def(Field_decl*);
  Decl* elaborate_def(Method_decl*);
  Decl* elaborate_def(Module_decl*);

  Stmt* elaborate(Stmt*);
  Stmt* elaborate(Empty_stmt*);
  Stmt* elaborate(Block_stmt*);
  Stmt* elaborate(Assign_stmt*);
  Stmt* elaborate(Return_stmt*);
  Stmt* elaborate(If_then_stmt*);
  Stmt* elaborate(If_else_stmt*);
  Stmt* elaborate(While_stmt*);
  Stmt* elaborate(Break_stmt*);
  Stmt* elaborate(Continue_stmt*);
  Stmt* elaborate(Expression_stmt*);
  Stmt* elaborate(Declaration_stmt*);

  void declare(Decl*);
  void redeclare(Decl*);
  void overload(Overload&, Decl*);

  Expr* call(Function_decl*, Expr_seq const&);
  Expr* resolve(Overload_expr*, Expr_seq const&);

  Overload* unqualified_lookup(Symbol const*);
  Overload* qualified_lookup(Scope*, Symbol const*);
  Overload* member_lookup(Record_decl*, Symbol const*);

  // Diagnostics
  void on_call_error(Expr_seq const&, Expr_seq const&, Type_seq const&);
  void locate(void const*, Location);
  Location locate(void const*);

  bool is_defining(Decl const*) const;

  // Found symbols.
  Function_decl* main = nullptr;

private:
  Location_map& locs;
  Symbol_table& syms;
  Scope_stack   stack;
  Decl_set      defined;
  Decl_stack    defining;
};


inline
Elaborator::Elaborator(Location_map& loc, Symbol_table& s)
  : locs(loc), syms(s)
{ }


inline void
Elaborator::locate(void const* p, Location l)
{
  locs.emplace(p, l);
}


inline Location
Elaborator::locate(void const* p)
{
  auto iter = locs.find(p);
  if (iter != locs.end())
    return iter->second;
  else
    return {};
}


// An RAII class that helps manage the scope stack.
// When constructed, a new scope is pushed on to
// the stack. On exit, that scope is removed.
struct Elaborator::Scope_sentinel
{
  // Initialize the new scope sentinel. A declaration
  // `d` can be associated with the new scope.
  Scope_sentinel(Elaborator& e, Decl* d = nullptr)
    : elab(e), take(false)
  {
    elab.stack.push(d);
  }

  // Push an existing scope onto the stack. Note that
  // this is not destroyed when the sentinel goes out
  // of scope.
  Scope_sentinel(Elaborator& e, Scope* s)
    : elab(e), take(true)
  {
    elab.stack.push(s);
  }

  ~Scope_sentinel()
  {
    if (take)
      elab.stack.take();
    else
      elab.stack.pop();
  }

  Elaborator& elab;
  bool        take;
};


// An RAII class used to manage a stack of definitions.
// This helps to prevent loops in recursive elaborations.
struct Elaborator::Defining_sentinel
{
  Defining_sentinel(Elaborator& e, Decl* d)
    : elab(e)
  {
    elab.defining.push_back(d);
  }

  ~Defining_sentinel()
  {
    elab.defining.pop_back();
  }

  Elaborator& elab;
};


#endif
