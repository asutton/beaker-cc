// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_ELABORATOR_HPP
#define BEAKER_ELABORATOR_HPP

#include "prelude.hpp"
#include "location.hpp"
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
struct Scope : Environment<Symbol const*, Decl*>
{
  Scope()
    : decl(nullptr)
  { }

  Scope(Decl* d)
    : decl(d)
  { }

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

  Type const* elaborate(Type const*);
  Type const* elaborate(Id_type const*);
  Type const* elaborate(Boolean_type const*);
  Type const* elaborate(Integer_type const*);
  Type const* elaborate(Function_type const*);
  Type const* elaborate(Reference_type const*);
  Type const* elaborate(Record_type const*);

  Expr* elaborate(Expr*);
  Expr* elaborate(Literal_expr*);
  Expr* elaborate(Id_expr*);
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
  Expr* elaborate(Member_expr* e);
  Expr* elaborate(Value_conv* e);
  Expr* elaborate(Default_init* e);
  Expr* elaborate(Copy_init* e);

  void elaborate(Decl*);
  void elaborate(Variable_decl*);
  void elaborate(Function_decl*);
  void elaborate(Parameter_decl*);
  void elaborate(Record_decl*);
  void elaborate(Field_decl*);
  void elaborate(Module_decl*);

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
