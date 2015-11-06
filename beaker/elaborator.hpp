// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_ELABORATOR_HPP
#define BEAKER_ELABORATOR_HPP

#include "prelude.hpp"
#include "location.hpp"
#include "overload.hpp"
#include "environment.hpp"

// The elaborator is responsible for a number of static
// analyses. In particular, it resolves identifiers and
// types expressions.
//
// FIXME: The type checking here is fundamentally broken.
// Instead of throwing exceptions, we should be documenting
// errors and continuing elaboration. There may be some
// cases where elaboration must stop.

#include <stack>
#include <unordered_map>
#include <vector>


// A scope defines a maximal lexical region of a program
// where no bindings are destroyed. A scope optionally
// assocaites a declaration with its bindings. This is
// used to maintain the current declaration context.
struct Scope : Environment<Symbol const*, Overload>
{
  Scope() 
    : decl(nullptr)
  { }

  Scope(Decl* d)
    : decl(d)
  { }

  Overload const& bind(Symbol const*, Decl*);

  Decl* decl;
};


// The scope stack maintains the current scope during
// elaboration. It adapts the more general stack to
// provide more language-specific names for those
// operations.
struct Scope_stack : Stack<Scope>
{
  Scope&       current()       { return top(); }
  Scope const& current() const { return top(); }

  Scope&       global()       { return bottom(); }
  Scope const& global() const { return bottom(); }

  Decl*          context() const;
  Module_decl*   module() const;
  Function_decl* function() const;

  void declare(Decl*);
};


// The elaborator is responsible for the annotation of
// an AST with type and other information.
class Elaborator
{
  struct Scope_sentinel;
public:
  Elaborator(Location_map&);

  Type const* elaborate(Expr*);
  Type const* elaborate(Literal_expr*);
  Type const* elaborate(Id_expr*);
  Type const* elaborate(Add_expr* e);
  Type const* elaborate(Sub_expr* e);
  Type const* elaborate(Mul_expr* e);
  Type const* elaborate(Div_expr* e);
  Type const* elaborate(Rem_expr* e);
  Type const* elaborate(Neg_expr* e);
  Type const* elaborate(Pos_expr* e);
  Type const* elaborate(Eq_expr* e);
  Type const* elaborate(Ne_expr* e);
  Type const* elaborate(Lt_expr* e);
  Type const* elaborate(Gt_expr* e);
  Type const* elaborate(Le_expr* e);
  Type const* elaborate(Ge_expr* e);
  Type const* elaborate(And_expr* e);
  Type const* elaborate(Or_expr* e);
  Type const* elaborate(Not_expr* e);
  Type const* elaborate(Call_expr* e);
  
  Type const* elaborate(Decl*);
  Type const* elaborate(Record_decl*);
  Type const* elaborate(Variable_decl*);
  Type const* elaborate(Function_decl*);
  Type const* elaborate(Parameter_decl*);
  Type const* elaborate(Module_decl*);

  // FIXME: Is there any real reason that these return
  // types? What is the type of an if statement?
  void elaborate(Stmt*);
  void elaborate(Empty_stmt*);
  void elaborate(Block_stmt*);
  void elaborate(Assign_stmt*);
  void elaborate(Return_stmt*);
  void elaborate(If_then_stmt*);
  void elaborate(If_else_stmt*);
  void elaborate(While_stmt*);
  void elaborate(Break_stmt*);
  void elaborate(Continue_stmt*);
  void elaborate(Expression_stmt*);
  void elaborate(Declaration_stmt*);


  // Found symbols.
  Function_decl* main = nullptr;

private:
  Location_map locs;
  Scope_stack  stack;
};


inline
Elaborator::Elaborator(Location_map& loc)
  : locs(loc)
{ }


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
