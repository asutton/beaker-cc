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


// The elaborator is responsible for the annotation of
// an AST with type and other information.
class Elaborator
{
  struct Scope_sentinel;
public:
  Elaborator(Location_map&, Symbol_table&);

  Type const* elaborate(Type const*);
  Type const* elaborate(Id_type const*);
  Type const* elaborate(Boolean_type const*);
  Type const* elaborate(Character_type const*);
  Type const* elaborate(Integer_type const*);
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
  Decl* elaborate_decl(Field_decl*);
  Decl* elaborate_decl(Method_decl*);

  Decl* elaborate_def(Decl*);
  Decl* elaborate_def(Field_decl*);
  Decl* elaborate_def(Method_decl*);

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

  // Diagnostics
  void on_call_error(Expr_seq const&, Expr_seq const&, Type_seq const&);
  void locate(void const*, Location);
  Location locate(void const*);

  // Found symbols.
  Function_decl* main = nullptr;

private:
  Location_map& locs;
  Symbol_table& syms;
  Scope_stack  stack;
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



struct Elaborator::Scope_sentinel
{
  Scope_sentinel(Elaborator& e, Decl* d = nullptr)
    : elab(e)
  {
    elab.stack.push(d);
  }

  ~Scope_sentinel()
  {
    elab.stack.pop();
  }

  Elaborator& elab;
};


#endif
