
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
// Mapping of names

// Synthesize a name using the linkage model for
// the declaration's language. Currently, there
// are two linkage models:
//
//    - C
//    - Beaker
//
// NOTE: Currently, these are the same. However, these
// differ be as Beaker evolves.
String
Generator::get_name(Decl const* d)
{
  if (d->is_foreign())
    return d->name()->spelling();
  else
    return d->name()->spelling();
}


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
    llvm::Type* operator()(Id_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Boolean_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Character_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Integer_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Function_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Array_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Block_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Reference_type const* t) const { return g.get_type(t); }
    llvm::Type* operator()(Record_type const* t) const { return g.get_type(t); }
  };
  return apply(t, Fn{*this});
}


// The program is unsound if we ever reach this
// function. Id-types are replaced with their
// referenced declarations during elaboration.
llvm::Type*
Generator::get_type(Id_type const* t)
{
  std::stringstream ss;
  ss << "unresolved id-type '" << *t->symbol() << '\'';
  throw std::runtime_error(ss.str());
}


// Return the 1 bit integer type.
llvm::Type*
Generator::get_type(Boolean_type const*)
{
  return build.getInt1Ty();
}


// Return the 8 bit integer type.
llvm::Type*
Generator::get_type(Character_type const*)
{
  return build.getInt8Ty();
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


// Return an array type.
llvm::Type*
Generator::get_type(Array_type const* t)
{
  llvm::Type* t1 = get_type(t->type());
  Value v = evaluate(t->extent());
  return llvm::ArrayType::get(t1, v.get_integer());
}


// A chunk is just a pointer to an object
// of the underlying type.
llvm::Type*
Generator::get_type(Block_type const* t)
{
  llvm::Type* t1 = get_type(t->type());
  return llvm::PointerType::getUnqual(t1);
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


// Return the structure type corresponding to the
// declaration of t.
llvm::Type*
Generator::get_type(Record_type const* t)
{
  return types.lookup(t->declaration())->second;
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
    llvm::Value* operator()(Member_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Index_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Value_conv const* e) const { return g.gen(e); }
    llvm::Value* operator()(Block_conv const* e) const { return g.gen(e); }
    llvm::Value* operator()(Default_init const* e) const { return g.gen(e); }
    llvm::Value* operator()(Copy_init const* e) const { return g.gen(e); }
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
  Type const* t = e->type();
  if (t == get_boolean_type())
    return build.getInt1(v.get_integer());
  if (t == get_character_type())
    return build.getInt8(v.get_integer());
  if (t == get_integer_type())
    return build.getInt32(v.get_integer());

  // FIXME: How should we generate array literals? Are
  // these global constants or are they local alloca
  // objects. Does it depend on context?

  // A string literal produces a new global string constant.
  // and returns a pointer to an array of N characters.
  if (is_string_type(t)) {
    Array_value a = v.get_array();
    String s = a.to_string();
    llvm::Constant* c = llvm::ConstantDataArray::getString(cxt, s);

    llvm::GlobalVariable* v = new llvm::GlobalVariable(
      *mod,                              // owning module
      c->getType(),                      // type
      true,                              // constant
      llvm::GlobalValue::PrivateLinkage, // private
      c                                  // initializer
    );

    // Allow globals with the same value to
    // be unified into the same object.
    v->setUnnamedAddr(true);
    return v;
  }

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
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Mul_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Div_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Rem_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Neg_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Pos_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Eq_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Ne_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Lt_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Gt_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Le_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Ge_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(And_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Or_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Not_expr const* e)
{
  throw std::runtime_error("not implemented");
}


llvm::Value*
Generator::gen(Call_expr const* e)
{
  throw std::runtime_error("not implemented");
}


// NOTE: The IR builder will automatically compact
// nested member expressions into a single GEP
// instruction. We don't have to do anything more
// complex than this.
llvm::Value*
Generator::gen(Member_expr const* e)
{
  llvm::Value* obj = gen(e->scope());
  std::vector<llvm::Value*> args {
    build.getInt32(0),            // 0th element from base
    build.getInt32(e->position()) // nth element in struct
  };
  return build.CreateGEP(obj, args);
}


llvm::Value*
Generator::gen(Index_expr const* e)
{
  llvm::Value* arr = gen(e->array());
  llvm::Value* ix = gen(e->index());
  std::vector<llvm::Value*> args {
    build.getInt32(0), // 0th element from base
    ix                 // requested index
  };
  return build.CreateGEP(arr, args);
}


llvm::Value*
Generator::gen(Value_conv const* e)
{
  llvm::Value* v = gen(e->source());
  return build.CreateLoad(v);
}


llvm::Value*
Generator::gen(Block_conv const* e)
{
  // Generate the array value.
  llvm::Value* a = gen(e->source());

  // Decay the array pointer to an array into
  // a pointer to the first object. This effectively
  // returns a pointer to the first object in the
  // array.
  llvm::Value *zero = build.getInt32(0);
  llvm::Value *args[] = { zero, zero };
  return build.CreateInBoundsGEP(a, args);
}


// TODO: Return the value or store it?
llvm::Value*
Generator::gen(Default_init const* e)
{
  Type const* t = e->type();
  llvm::Type* type = get_type(t);

  // Scalar types should get a 0 value in the
  // appropriate type.
  if (is_scalar_type(t))
    return llvm::ConstantInt::get(type, 0);

  // Aggregate types are zero initialized.
  //
  // NOTE: This isn't actually correct. Aggregate types
  // should be memberwise default initialized.
  if (is_aggregate_type(t))
    return llvm::ConstantAggregateZero::get(type);

  throw std::runtime_error("unhahndled default initializer");
}


// TODO: Return the value or store it?
llvm::Value*
Generator::gen(Copy_init const* e)
{
  return gen(e->value());
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
  throw std::runtime_error("not implemented");
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
  build.CreateRet(v);
}


void
Generator::gen(If_then_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(If_else_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(While_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Break_stmt const* s)
{
  throw std::runtime_error("not implemented");
}


void
Generator::gen(Continue_stmt const* s)
{
  throw std::runtime_error("not implemented");
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
    void operator()(Record_decl const* d) { return g.gen(d); }
    void operator()(Field_decl const* d) { return g.gen(d); }
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
  String      name = get_name(d);
  llvm::Type* type = get_type(d->type());

  // Try to generate a constant initializer.
  llvm::Constant* init = nullptr;
  if (!d->is_foreign()) {

    // FIXME: If the initializer can be reduced to a value,
    // then generate that constant. If not, we need dynamic
    // initialization of global variables.

    init = llvm::Constant::getNullValue(type);

    // llvm::Value* val = gen(d->init());
    // if (llvm::Constant* c = llvm::dyn_cast<llvm::Constant>(val)) {
    //   init = c;
    // } 
  }


  // Note that the aggregate 0 only applies to aggregate
  // types. We can't apply it to initializers for scalars.

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
  String name = get_name(d);
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

  // If the declaration is not defined, then don't
  // do any of this stuff...
  if (!d->body())
    return;

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

  // Generate the body of the function.
  gen(d->body());
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


// Generate a new struct type.
void
Generator::gen(Record_decl const* d)
{
  // If the record is empty, generate a struct
  // with exactly one byte so that we never have
  // a type with 0 size.
  std::vector<llvm::Type*> ts;
  if (d->fields().empty()) {
    ts.push_back(build.getInt8Ty());
  } else {
    for (Decl const* f : d->fields())
      ts.push_back(get_type(f->type()));
  }

  // This will automatically be added to the module,
  // but if it's not used, then it won't be generated.
  llvm::Type* t = llvm::StructType::create(cxt, ts, d->name()->spelling());
  types.bind(d, t);
}


void
Generator::gen(Field_decl const* d)
{
  // NOTE: We should never actually get here.
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
