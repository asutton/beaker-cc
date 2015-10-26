// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_GENERATOR_HPP
#define BEAKER_GENERATOR_HPP

// The LLVM IR generator.

#include "prelude.hpp"

#include "llvm/prelude.hpp"


namespace ll = lingo::llvm;


struct Generator
{
  ll::Type const* gen(Type const* t);
  ll::Type const* gen(Boolean_type const* t);
  ll::Type const* gen(Integer_type const* t);
  ll::Type const* gen(Function_type const* t);
  
  void gen(Expr const* e);
  void gen(Literal_expr const* e);
  void gen(Id_expr const* e);
  void gen(Add_expr const* e);
  void gen(Sub_expr const* e);
  void gen(Mul_expr const* e);
  void gen(Div_expr const* e);
  void gen(Rem_expr const* e);
  void gen(Neg_expr const* e);
  void gen(Pos_expr const* e);
  void gen(Eq_expr const* e);
  void gen(Ne_expr const* e);
  void gen(Lt_expr const* e);
  void gen(Gt_expr const* e);
  void gen(Le_expr const* e);
  void gen(Ge_expr const* e);
  void gen(And_expr const* e);
  void gen(Or_expr const* e);
  void gen(Not_expr const* e);
  void gen(Call_expr const* e);
  
  void gen(Stmt const* s);
  void gen(Empty_stmt const* s);
  void gen(Block_stmt const* s);
  void gen(Return_stmt const* s);
  void gen(Expression_stmt const* s);
  void gen(Declaration_stmt const* s);

  void gen(Decl const* d);
  void gen(Variable_decl const*);
  void gen(Function_decl const*);
  void gen(Parameter_decl const*);
  void gen(Module_decl const*);
};


#endif
