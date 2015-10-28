
#include "generator.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "decl.hpp"

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


// -------------------------------------------------------------------------- //
// Code generation for expressions
//
// An expression is transformed into a sequence instructions whose
// intermediate results are saved in registers.

void
Generator::gen(Expr const* e)
{
  struct Fn
  {
    Generator& g;
    void operator()(Literal_expr const* e) const { g.gen(e); }
    void operator()(Id_expr const* e) const { g.gen(e); }
    void operator()(Add_expr const* e) const { g.gen(e); }
    void operator()(Sub_expr const* e) const { g.gen(e); }
    void operator()(Mul_expr const* e) const { g.gen(e); }
    void operator()(Div_expr const* e) const { g.gen(e); }
    void operator()(Rem_expr const* e) const { g.gen(e); }
    void operator()(Neg_expr const* e) const { g.gen(e); }
    void operator()(Pos_expr const* e) const { g.gen(e); }
    void operator()(Eq_expr const* e) const { g.gen(e); }
    void operator()(Ne_expr const* e) const { g.gen(e); }
    void operator()(Lt_expr const* e) const { g.gen(e); }
    void operator()(Gt_expr const* e) const { g.gen(e); }
    void operator()(Le_expr const* e) const { g.gen(e); }
    void operator()(Ge_expr const* e) const { g.gen(e); }
    void operator()(And_expr const* e) const { g.gen(e); }
    void operator()(Or_expr const* e) const { g.gen(e); }
    void operator()(Not_expr const* e) const { g.gen(e); }
    void operator()(Call_expr const* e) const { g.gen(e); }
  };

  return apply(e, Fn{*this});
}


void 
Generator::gen(Literal_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Id_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Add_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Sub_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Mul_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Div_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Rem_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Neg_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Pos_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Eq_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Ne_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Lt_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Gt_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Le_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Ge_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(And_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Or_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Not_expr const* e)
{
  throw std::runtime_error("not implemented");
}


void 
Generator::gen(Call_expr const* e)
{
  throw std::runtime_error("not implemented");
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
    void operator()(Return_stmt const* s) { g.gen(s); }
    void operator()(Expression_stmt const* s) { g.gen(s); }
    void operator()(Declaration_stmt const* s) { g.gen(s); }
  };
  apply(s, Fn{*this});
}


void
Generator::gen(Empty_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Block_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Return_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Expression_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Declaration_stmt const* s)
{
  throw std::runtime_error("not implemented");
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
  throw std::runtime_error("not implemented");
}


void
Generator::gen_global(Variable_decl const* d)
{
  String const&   name = d->name()->spelling();
  llvm::Type*     type = build.getInt32Ty();
  llvm::Constant* init = llvm::ConstantAggregateZero::get(type);

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

  // Establish a new binding environment for declarations
  // related to this function.
  Symbol_sentinel scope(*this);
  
  // Set the names of function arguments and establish
  // bindings for all of them.
  {
    auto ai = fn->arg_begin();
    auto pi = d->parameters().begin();
    while (ai != fn->arg_end()) {
      Decl const* p = *pi;
      llvm::Argument* a = &*ai;
      a->setName(p->name()->spelling());

      // Establish the symbol binding for the parameter.
      stack.top().bind(p, a);

      ++ai;
      ++pi;
    }
  }

  // TODO: Generate the basic blocks and body
  // of the function.
  (void)fn;
}


void
Generator::gen(Parameter_decl const*)
{
  // NOTE: We don't visit parameters independently
  // of function declarations, so we should never
  // reach here.
  throw std::runtime_error("unreachable");
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

