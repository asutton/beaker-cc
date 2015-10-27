
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


// -------------------------------------------------------------------------- //
// Mapping of types
//
// The type generator transforms a beaker type into
// its correspondiong LLVM type.
//
// TODO: Should this be a part of generator? It's more
// of a transformation and does not appear to rely on
// basic context.


#if 0
Generator::gen(Type const* t)
{
  struct Fn
  {
    Generator& g;
    ll::Type const* operator()(Boolean_type const* t) const { return g.gen(t); }
    ll::Type const* operator()(Integer_type const* t) const { return g.gen(t); }
    ll::Type const* operator()(Function_type const* t) const { return g.gen(t); }
  };
  return apply(t, Fn{*this});
}


ll::Type const*
Generator::gen(Boolean_type const*)
{
  return ll::get_boolean_type();
}


ll::Type const*
Generator::gen(Integer_type const*)
{
  return ll::get_integer_type(32);
}


ll::Type const*
Generator::gen(Function_type const*)
{
  // FIXME: Implement mem.
  throw std::runtime_error("not implemented");      
}
#endif


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
Generator::gen(Variable_decl const* d)
{
  String const&   name = d->name()->spelling();
  llvm::Type*     type = build.getInt32Ty();
  llvm::Constant* init = llvm::ConstantAggregateZero::get(type);

  new llvm::GlobalVariable(
    *mod,                                  // owning module
    type,                                  // type
    false,                                 // is constant
    llvm::GlobalVariable::ExternalLinkage, // linkage,
    init,                                  // initializer
    name                                   // name
  );
} 


void
Generator::gen(Function_decl const*)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Parameter_decl const*)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Module_decl const* d)
{
  // Initialize the module.
  mod = new llvm::Module("b.out", cxt);

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

