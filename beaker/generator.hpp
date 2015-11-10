// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_GENERATOR_HPP
#define BEAKER_GENERATOR_HPP

// The LLVM IR generator.

#include "prelude.hpp"
#include "environment.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>


// Used to maintain a mapping of Beaker declarations
// to their corresponding LLVM declarations. This is
// used to track the names of globals and parameters.
//
// TODO: If we support local variables, this will not be 
// sufficient for code generation. We would need to bind 
// local variable names to their adresses --- the results
// returned by their alloca instructions. Of course,
// alloca instructions are inherently values, so maybe
// this isn't as bad as I think? We may have to downcast
// to resolve the symbol kind (global, function, or
// local).
using Symbol_env = Environment<Decl const*, llvm::Value*>;
using Symbol_stack = Stack<Symbol_env>;


struct Generator
{
  Generator();

  llvm::Module* operator()(Decl const*);

  llvm::Type* get_type(Type const*);
  llvm::Type* get_type(Boolean_type const*);
  llvm::Type* get_type(Integer_type const*);
  llvm::Type* get_type(Function_type const*);
  llvm::Type* get_type(Reference_type const*);

  llvm::Value* gen(Expr const*);
  llvm::Value* gen(Literal_expr const*);
  llvm::Value* gen(Id_expr const*);
  llvm::Value* gen(Add_expr const*);
  llvm::Value* gen(Sub_expr const*);
  llvm::Value* gen(Mul_expr const*);
  llvm::Value* gen(Div_expr const*);
  llvm::Value* gen(Rem_expr const*);
  llvm::Value* gen(Neg_expr const*);
  llvm::Value* gen(Pos_expr const*);
  llvm::Value* gen(Eq_expr const*);
  llvm::Value* gen(Ne_expr const*);
  llvm::Value* gen(Lt_expr const*);
  llvm::Value* gen(Gt_expr const*);
  llvm::Value* gen(Le_expr const*);
  llvm::Value* gen(Ge_expr const*);
  llvm::Value* gen(And_expr const*);
  llvm::Value* gen(Or_expr const*);
  llvm::Value* gen(Not_expr const*);
  llvm::Value* gen(Call_expr const*);
  llvm::Value* gen(Value_conv const*);
  
  void gen(Stmt const*);
  void gen(Empty_stmt const*);
  void gen(Block_stmt const*);
  void gen(Assign_stmt const*);
  void gen(Return_stmt const*);
  void gen(If_then_stmt const*);
  void gen(If_else_stmt const*);
  void gen(While_stmt const*);
  void gen(Break_stmt const*);
  void gen(Continue_stmt const*);
  void gen(Expression_stmt const*);
  void gen(Declaration_stmt const*);

  void gen(Decl const*);
  void gen(Variable_decl const*);
  void gen_local(Variable_decl const*);
  void gen_global(Variable_decl const*);
  void gen(Function_decl const*);
  void gen(Parameter_decl const*);
  void gen(Module_decl const*);

  llvm::LLVMContext cxt;
  llvm::IRBuilder<> build;
  llvm::Module*     mod;
  llvm::Value*      ret;

  Symbol_stack      stack;

  struct Symbol_sentinel;
};


inline
Generator::Generator()
  : cxt(), build(cxt), mod(nullptr)
{ }


// An RAII class used to manage the registration and
// removal of name-to-value bindings for code generation.
struct Generator::Symbol_sentinel
{
  Symbol_sentinel(Generator& g)
    : gen(g)
  {
    gen.stack.push();
  }

  ~Symbol_sentinel()
  {
    gen.stack.pop();
  }

  Generator& gen;
};


#endif
