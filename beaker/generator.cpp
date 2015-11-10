
#include "generator.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "decl.hpp"
#include "evaluator.hpp"

#include "llvm/IR/Type.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"

#include <iostream>


// -------------------------------------------------------------------------- //
// Mapping of types
//
// The type generator transforms a beaker type into
// its correspondiong LLVM type.


llvm::Type*
Generator::get_type(Type const* t)
{
  struct Fn
  {
    Generator& g;
    llvm::Type* operator()(Boolean_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Integer_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Function_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Reference_type const* t) const { return g.get_type(t); }
  };
  return apply(t, Fn{*this});
}


// Return the 1 bit integer type.
llvm::Type*
Generator::get_type(Boolean_type const*)
{
  return build.getInt1Ty();
}


// Return the 32 bit integer type.
llvm::Type*
Generator::get_type(Integer_type const*)
{
  return build.getInt32Ty();
}


// Return a function type.
llvm::Type*
Generator::get_type(Function_type const* t)
{
  std::vector<llvm::Type*> ts;
  ts.reserve(t->parameter_types().size());
  for (Type const* t1 : t->parameter_types())
    ts.push_back(get_type(t1));
  llvm::Type* r = get_type(t->return_type());
  return llvm::FunctionType::get(r, ts, false);
}


// Translate reference types into pointer types in the
// generic address space.
//
// TODO: Actually do this?
llvm::Type*
Generator::get_type(Reference_type const* t)
{
  llvm::Type* t1 = get_type(t->type());
  return llvm::PointerType::getUnqual(t1);
}


// -------------------------------------------------------------------------- //
// Code generation for expressions
//
// An expression is transformed into a sequence instructions whose
// intermediate results are saved in registers.

llvm::Value*
Generator::gen(Expr const* e)
{
  struct Fn
  {
    Generator& g;
    llvm::Value* operator()(Literal_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Id_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Add_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Sub_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Mul_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Div_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Rem_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Neg_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Pos_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Eq_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Ne_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Lt_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Gt_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Le_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Ge_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(And_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Or_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Not_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Call_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Value_conv const* e) const { return g.gen(e); }
  };

  return apply(e, Fn{*this});
}


// Return the value corresponding to a literal expression.
llvm::Value*
Generator::gen(Literal_expr const* e)
{
  // TODO: Write better type queries.
  //
  // TODO: Write a better interface for values.
  Value v = evaluate(e);
  if (e->type() == get_boolean_type())
    return build.getInt1(v.get_integer());
  if (e->type() == get_integer_type())
    return build.getInt32(v.get_integer());
  else
    throw std::runtime_error("cannot generate function literal");
}


// Returns the value associated with the declaration.
//
// TODO: Do we need to do anything different for function
// identifiers or not?
llvm::Value*
Generator::gen(Id_expr const* e)
{
  return stack.lookup(e->declaration())->second;
}


llvm::Value*
Generator::gen(Add_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateAdd(l, r);
}


llvm::Value*
Generator::gen(Sub_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateSub(left,right);
}


llvm::Value*
Generator::gen(Mul_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateMul(left,right);
}


llvm::Value*
Generator::gen(Div_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateSDiv(left,right);
}


llvm::Value*
Generator::gen(Rem_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateSRem(left,right);
}


llvm::Value*
Generator::gen(Neg_expr const* e)
{
  llvm::Value* v1 = build.getInt32(0);
  llvm::Value* v2 = gen(e->first);
  return build.CreateSub(v1,v2);

}


llvm::Value*
Generator::gen(Pos_expr const* e)
{
  llvm::Value* v1 = build.getInt32(0);
  llvm::Value* v2 = gen(e->first);
  return build.CreateAdd(v1,v2);
}


llvm::Value*
Generator::gen(Eq_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateICmpEQ(left,right);
}


llvm::Value*
Generator::gen(Ne_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateICmpNE(left,right);
}


llvm::Value*
Generator::gen(Lt_expr const* e)
{ 
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateICmpSLT(left,right);
}


llvm::Value*
Generator::gen(Gt_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateICmpSGT(left,right);
}


llvm::Value*
Generator::gen(Le_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateICmpSLE(left,right);
}


llvm::Value*
Generator::gen(Ge_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateICmpSGE(left,right);
}


llvm::Value*
Generator::gen(And_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateAnd(left,right);
}


llvm::Value*
Generator::gen(Or_expr const* e)
{
  llvm::Value* left = gen(e->left());
  llvm::Value* right = gen(e->right());
  return build.CreateOr(left,right);
}


llvm::Value*
Generator::gen(Not_expr const* e)
{
  llvm::Value* op = gen(e->operand());
  return build.CreateNot(op);
}


llvm::Value*
Generator::gen(Call_expr const* e)
{
  llvm::Value* callee = gen(e->target());
  if(!callee) throw std::runtime_error("!callee");


  std::vector<llvm::Value*> args;
  for(auto a : e->arguments()) {
    args.push_back(gen(a));
  }


  return build.CreateCall(callee, args);
}


llvm::Value*
Generator::gen(Value_conv const* e)
{
  llvm::Value* v = gen(e->source());
  return build.CreateLoad(v);
}


// -------------------------------------------------------------------------- //
// Code generation for statements
//
// The statement generator is responsible for
// the generation of statements at block scope.

void
Generator::gen(Stmt const* s)
{
  struct Fn
  {
    Generator& g;
    void operator()(Empty_stmt const* s) { g.gen(s); }
    void operator()(Block_stmt const* s) { g.gen(s); }
    void operator()(Assign_stmt const* s) { g.gen(s); }
    void operator()(Return_stmt const* s) { g.gen(s); }
    void operator()(If_then_stmt const* s) { g.gen(s); }
    void operator()(If_else_stmt const* s) { g.gen(s); }
    void operator()(While_stmt const* s) { g.gen(s); }
    void operator()(Break_stmt const* s) { g.gen(s); }
    void operator()(Continue_stmt const* s) { g.gen(s); }
    void operator()(Expression_stmt const* s) { g.gen(s); }
    void operator()(Declaration_stmt const* s) { g.gen(s); }
  };
  apply(s, Fn{*this});
}


void
Generator::gen(Empty_stmt const* s)
{
  throw std::runtime_error("empty: not implemented");
}


// Generate code for a sequence of statements.
// Note that this does not correspond to a basic
// block since we don't need any terminators
// in the following program.
//
//    {
//      { ; }
//    }
//
// We only need new blocks for specific control
// flow concepts.
void
Generator::gen(Block_stmt const* s)
{
  for (Stmt const* s1 : s->statements())
    gen(s1);
}


void
Generator::gen(Assign_stmt const* s)
{
  llvm::Value* lhs = gen(s->object());
  llvm::Value* rhs = gen(s->value());
  build.CreateStore(rhs, lhs);
}


void
Generator::gen(Return_stmt const* s)
{
  llvm::Value* v = gen(s->value());
  
  build.CreateStore(v, ret);
}


void
Generator::gen(If_then_stmt const* s)
{
  llvm::Value* cond = gen(s->condition());
  if(!cond) {
    throw std::runtime_error("err:cond");
  }
  
  llvm::Function* theFunc = build.GetInsertBlock()->getParent();
  llvm::BasicBlock* thenBB = 
    llvm::BasicBlock::Create(cxt, "then", theFunc);
  llvm::BasicBlock* mergeBB =
    llvm::BasicBlock::Create(cxt, "merge");

  build.CreateCondBr(cond, thenBB, mergeBB);

  //if true
  build.SetInsertPoint(thenBB);
  gen(s->body());
  build.CreateBr(mergeBB);

  //merge
  theFunc->getBasicBlockList().push_back(mergeBB);
  build.SetInsertPoint(mergeBB);

  //throw std::runtime_error("if_then: not implemented");
}


void
Generator::gen(If_else_stmt const* s)
{
  llvm::Value* cond = gen(s->condition());
  if(!cond) {
    throw std::runtime_error("err:cond");
  }
  
  llvm::Function* theFunc = build.GetInsertBlock()->getParent();
  llvm::BasicBlock* thenBB = 
    llvm::BasicBlock::Create(cxt, "then", theFunc);
  llvm::BasicBlock* elseBB =
    llvm::BasicBlock::Create(cxt, "else");
  llvm::BasicBlock* mergeBB =
    llvm::BasicBlock::Create(cxt, "merge");

  build.CreateCondBr(cond, thenBB, elseBB);

  //if true
  build.SetInsertPoint(thenBB);
  gen(s->true_branch());
  build.CreateBr(mergeBB);

  //else false
  theFunc->getBasicBlockList().push_back(elseBB);
  build.SetInsertPoint(elseBB);
  gen(s->false_branch());
  build.CreateBr(mergeBB);

  //merge
  theFunc->getBasicBlockList().push_back(mergeBB);
  build.SetInsertPoint(mergeBB);
  //throw std::runtime_error("if_else: not implemented");
}


void
Generator::gen(While_stmt const* s)
{
  throw std::runtime_error("while: not implemented");
}


void
Generator::gen(Break_stmt const* s)
{
  throw std::runtime_error("break: not implemented");
}


void
Generator::gen(Continue_stmt const* s)
{
  throw std::runtime_error("continue: not implemented");
}


void
Generator::gen(Expression_stmt const* s)
{
  gen(s->expression());
}


void
Generator::gen(Declaration_stmt const* s)
{
  gen(s->declaration());
}


// -------------------------------------------------------------------------- //
// Code generation for declarations
//
// TODO: We can't generate all of the code for a module
// in a single pass. We probably need to break this up
// into a number of smaller declaration generators. For
// example, generators that:
//
//    - produce declarations
//    - produce global initializers
//    - produce global destructors
//    - other stuff
//
// In, it might not be worthwhile to have a number
// of sub-generators that refer to the top-level
// generator.

void
Generator::gen(Decl const* d)
{
  struct Fn
  {
    Generator& g;
    void operator()(Variable_decl const* d) { return g.gen(d); }
    void operator()(Function_decl const* d) { return g.gen(d); }
    void operator()(Parameter_decl const* d) { return g.gen(d); }
    void operator()(Module_decl const* d) { return g.gen(d); }
  };
  return apply(d, Fn{*this});
}


void
Generator::gen_local(Variable_decl const* d)
{
  throw std::runtime_error("gen_local: not implemented");
}


void
Generator::gen_global(Variable_decl const* d)
{
  String const&   name = d->name()->spelling();
  llvm::Type*     type = get_type(d->type());

  // FIXME: Handle initialization correctly. If the
  // initializer is a literal (or a constant expression),
  // then we should evaluate that and assign it here.
  llvm::Constant* init = llvm::ConstantInt::get(type, 0);

  // Note that the aggregate 0 only applies to aggregate
  // types. We can't apply it to initializers for scalars.
  // llvm::Constant* init = llvm::ConstantAggregateZero::get(type);

  // Build the global variable, automatically adding
  // it to the module.
  llvm::GlobalVariable* var = new llvm::GlobalVariable(
    *mod,                                  // owning module
    type,                                  // type
    false,                                 // is constant
    llvm::GlobalVariable::ExternalLinkage, // linkage,
    init,                                  // initializer
    name                                   // name
  );

  // Create a binding for the new variable.
  stack.top().bind(d, var);
}


// Generate code for a variable declaration. Note that
// code generation depends heavily on context. Globals
// and locals are very different.
//
// TODO: If we add class/record types, then we also
// need to handle member variables as well. Maybe.
void
Generator::gen(Variable_decl const* d)
{
  if (is_global_variable(d))
    return gen_global(d);
  else
    return gen_local(d);
}


void
Generator::gen(Function_decl const* d)
{
  String const& name = d->name()->spelling();
  llvm::Type*   type = get_type(d->type());

  // Build the function.
  llvm::FunctionType* ftype = llvm::cast<llvm::FunctionType>(type);
  llvm::Function* fn = llvm::Function::Create(
    ftype,                           // function type
    llvm::Function::ExternalLinkage, // linkage
    name,                            // name
    mod);                            // owning module

  // Create a new binding for the variable.

  stack.top().bind(d, fn);

  // Establish a new binding environment for declarations
  // related to this function.
  Symbol_sentinel scope(*this);

  // Build the argument list. Note that
  {
    auto ai = fn->arg_begin();
    auto pi = d->parameters().begin();
    while (ai != fn->arg_end()) {
      Decl const* p = *pi;
      llvm::Argument* a = &*ai;
      a->setName(p->name()->spelling());

      // Create an initial name binding for the function
      // parameter. Note that we're going to overwrite
      // this when we create locals for each parameter.
      stack.top().bind(p, a);

      ++ai;
      ++pi;
    }
  }

  // Build the entry point for the function
  // and make that the insertion point.
  //
  // TODO: We probably need a stack of blocks
  // so that we know where we are.
  llvm::BasicBlock* b = llvm::BasicBlock::Create(cxt, "b", fn);
  build.SetInsertPoint(b);

  // Generate a local variable for each of the variables.
  for (Decl const* p : d->parameters())
    gen(p);

  llvm::Type* t = get_type(d->return_type());
  ret = build.CreateAlloca(t);


  // Generate the body of the function.
  gen(d->body());

  llvm::Value* loadret = build.CreateLoad(ret);
  build.CreateRet(loadret);
}


void
Generator::gen(Parameter_decl const* d)
{
  llvm::Type* t = get_type(d->type());
  llvm::Value* a = stack.top().get(d).second;
  llvm::Value* v = build.CreateAlloca(t);
  stack.top().rebind(d, v);
  build.CreateStore(a, v);
}


void
Generator::gen(Module_decl const* d)
{
  // Establish the global binding environment.
  Symbol_sentinel scope(*this);

  // Initialize the module.
  //
  // TODO: Make the output name the ".ll" version of the
  // the input name. Although this might also depend on
  // whether we're generating IR or object code?
  assert(!mod);
  mod = new llvm::Module("a.ll", cxt);

  // Generate all top-level declarations.
  for (Decl const* d1 : d->declarations())
    gen(d1);

  // TODO: Make a second pass to generate global
  // constructors for initializers.
}


llvm::Module*
Generator::operator()(Decl const* d)
{
  assert(is<Module_decl>(d));
  gen(d);
  return mod;
}
