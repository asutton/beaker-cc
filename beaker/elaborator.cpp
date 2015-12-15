// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/elaborator.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/convert.hpp"
#include "beaker/evaluator.hpp"
#include "beaker/error.hpp"

#include <algorithm>
#include <iostream>


// -------------------------------------------------------------------------- //
// Declaration of entities


// Determine if d can be overloaded with the existing
// elements in the set.
void
Elaborator::overload(Overload& ovl, Decl* curr)
{
  // Check to make sure that curr does not conflict with any
  // declarations in the current overload set.
  for (Decl* prev : ovl) {
    // If the two declarations have the same type, this
    // is not overloading. It is redefinition.
    if (prev->type() == curr->type()) {
      std::stringstream ss;
      ss << "redefinition of " << *curr->name() << '\n';
      throw Type_error({}, ss.str());
    }

    if (!can_overload(prev, curr)) {
      std::stringstream ss;
      ss << "cannot overload " << *curr->name() << '\n';
      throw Type_error({}, ss.str());
    }
  }

  ovl.push_back(curr);
}


// Create a declarative binding for d. This also checks
// that the we are not redefining a symbol in the current
// scope.
void
Elaborator::declare(Decl* d)
{
  Scope& scope = stack.current();

  // Set d's declaration context.
  d->cxt_ = stack.context();

  // If we've already seen the name, we should
  // determine if it can be overloaded.
  if (Scope::Binding* bind = scope.lookup(d->name()))
    return overload(bind->second, d);

  // Create a new overload set.
  Scope::Binding& bind = scope.bind(d->name(), {});
  Overload& ovl = bind.second;
  ovl.push_back(d);
}


// When opening the scope of a previously declared
// entity, simply push the declaration into its
// overload set.
void
Elaborator::redeclare(Decl* d)
{
  Scope& scope = stack.current();
  Overload* ovl;
  if (Scope::Binding* bind = scope.lookup(d->name()))
    ovl = &bind->second;
  else
    ovl = &scope.bind(d->name(), {}).second;
  ovl->push_back(d);
}


// Perform lookup of an unqualified identifier. This
// will search enclosing scopes for the innermost
// binding of the identifier.
Overload*
Elaborator::unqualified_lookup(Symbol const* sym)
{
  if (Scope::Binding* bind = stack.lookup(sym))
    return &bind->second;
  else
    return nullptr;
}


// Perform a qualified lookup of a name in the given scope. 
// This searches only that scope for a binding for the identifier. 
// If the scope is that of a record, the name may be a member of
// a base class.
Overload*
Elaborator::qualified_lookup(Scope* s, Symbol const* sym)
{
  if (Record_decl* d = as<Record_decl>(s->decl))
    return member_lookup(d, sym);
  
  if (Scope::Binding* bind = s->lookup(sym))
    return &bind->second;
  
  return nullptr;
}



Overload*
Elaborator::member_lookup(Record_decl* d, Symbol const* sym)
{
  do {
    if (Scope::Binding* bind = d->scope()->lookup(sym))
      return &bind->second;
    d = d->base_declaration();
  } while (d);
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Elaboration of types


inline bool
requires_definition(Type const* t)
{
  // A type T[n] requires a definition iff T requires
  // a definition.
  if (Array_type const* a = as<Array_type>(t))
    return requires_definition(a->type());

  // A user-defined type T requires a definition iff it
  // is not a reference or block type.
  if (is<Record_type>(t))
    return true;
  return false;
}


// Elaborate a type. If the type is requried to be a complete
// type then recursively elaborate it.
Type const*
Elaborator::elaborate_type(Type const* t)
{
  Type const* t1 = elaborate(t);
  if (requires_definition(t1))
    t1 = elaborate_def(t1);
  return t1;
}


Type const*
Elaborator::elaborate_def(Type const* t)
{
  if (Record_type const* r = as<Record_type>(t)) {
    elaborate_def(r->declaration());
    return t;
  }
  lingo_unreachable();
}



Type const*
Elaborator::elaborate(Type const* t)
{
  struct Fn
  {
    Elaborator& elab;

    Type const* operator()(Id_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Boolean_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Character_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Integer_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Float_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Double_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Function_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Block_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Array_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Reference_type const* t) { return elab.elaborate(t); }
    Type const* operator()(Record_type const* t) { return elab.elaborate(t); }
  };
  return apply(t, Fn{*this});
}


// The elaboration of an identifer as a type performs
// unqualified name lookup. The declaration associated
// with the name shall be a user-defined type or an
// alias.
//
// TODO: Support type aliases.
Type const*
Elaborator::elaborate(Id_type const* t)
{
  // Perform unqualified lookup.
  Overload* ovl = unqualified_lookup(t->symbol());
  if (!ovl) {
    String msg = format("no matching declaration for '{}'", *t);
    throw Lookup_error(locate(t), msg);
  }

  // We can't currently overload types, so this could
  // only mean that we found a funtion overload set.
  if (ovl->size() > 1) {
    String msg = format("'{}' does not name a type", *t);
    throw Lookup_error(locate(t), msg);
  }

  // Determine if the name is a type declaration.
  Decl* d = ovl->front();
  if (Record_decl* r = as<Record_decl>(d))
    return get_record_type(r);

  String msg = format("'{}' does not name a type", *t);
  throw Lookup_error(locate(t), msg);
}


Type const*
Elaborator::elaborate(Boolean_type const* t)
{
  return t;
}


Type const*
Elaborator::elaborate(Character_type const* t)
{
  return t;
}

Type const*
Elaborator::elaborate(Integer_type const* t)
{
  return t;
}

Type const*
Elaborator::elaborate(Float_type const* t)
{
  return t;
}

Type const*
Elaborator::elaborate(Double_type const* t)
{
  return t;
}

// Elaborate each type in the function type.
Type const*
Elaborator::elaborate(Function_type const* t)
{
  Type_seq ts;
  ts.reserve(t->parameter_types().size());
  for (Type const* t1 : t->parameter_types())
    ts.push_back(elaborate(t1));
  Type const* r = elaborate(t->return_type());
  return get_function_type(ts, r);
}


Type const*
Elaborator::elaborate(Array_type const* t)
{
  Type const* t1 = elaborate(t->type());
  Expr* e = elaborate(t->extent());
  Expr* n = reduce(e);
  if (!n)
    throw Type_error({}, "non-constant array extent");
  return get_array_type(t1, n);
}

Type const*
Elaborator::elaborate(Block_type const* t)
{
  Type const* t1 = elaborate(t->type());
  return get_block_type(t1);
}


Type const*
Elaborator::elaborate(Reference_type const* t)
{
  Type const* t1 = elaborate(t->type());
  return get_reference_type(t1);
}


// No further elaboration is needed.
Type const*
Elaborator::elaborate(Record_type const* t)
{
  return t;
}


// -------------------------------------------------------------------------- //
// Elaboration of expressions

// Returns the type of an expression. This also annotates
// the expression by saving the computed type as part of
// the expression.
Expr*
Elaborator::elaborate(Expr* e)
{
  struct Fn
  {
    Elaborator& elab;

    Expr* operator()(Literal_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Id_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Decl_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Add_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Sub_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Mul_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Div_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Rem_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Neg_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Pos_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Eq_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Ne_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Lt_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Gt_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Le_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Ge_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(And_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Or_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Not_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Call_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Dot_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Field_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Method_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Index_expr* e) const { return elab.elaborate(e); }
    Expr* operator()(Value_conv* e) const { return elab.elaborate(e); }
    Expr* operator()(Block_conv* e) const { return elab.elaborate(e); }
    Expr* operator()(Base_conv* e) const { return elab.elaborate(e); }
    Expr* operator()(Promote_conv* e) const { return elab.elaborate(e); }
    Expr* operator()(Default_init* e) const { return elab.elaborate(e); }
    Expr* operator()(Trivial_init* e) const { return elab.elaborate(e); }
    Expr* operator()(Copy_init* e) const { return elab.elaborate(e); }
    Expr* operator()(Reference_init* e) const { return elab.elaborate(e); }
  };

  return apply(e, Fn{*this});
}


// Literal expressions are fully elaborated at the point
// of construction.
Expr*
Elaborator::elaborate(Literal_expr* e)
{
  return e;
}


// The elaboration of an identifier requires performs
// unqualified name lookup. The associated declaration
// shall not declare a type.
//
// If lookup associates a single declaration D, with
// declared type T, with the name, then the type of
// the expression is determined as follows:
//
//  - if D is an object, the  type of the expression
//    is T&;
//  - otherwise, then the type is T.
//
// Lookup may associate a set of declarations (an
// overload set). A single declaration is selected
// by overload resolution (see call expressions).
//
// TODO: Allow overload sets of templates?
Expr*
Elaborator::elaborate(Id_expr* e)
{
  Location loc = locate(e);

  // Lookup the declaration for the identifier.
  Overload* ovl = unqualified_lookup(e->symbol());
  if (!ovl) {
    std::stringstream ss;
    ss << "no matching declaration for '" << *e->symbol() << '\'';
    throw Lookup_error(locs.get(e), ss.str());
  }

  // We can't resolve an overload without context,
  // so return the resolved overload set.
  if (ovl->size() > 1) {
    Expr* ret = new Overload_expr(ovl);
    locate(ret, loc);
    return ret;
  }

  // Get the declaration named by the symbol.
  Decl* d = ovl->front();

  // An identifier always refers to an object, so
  // these expressions have reference type.
  Type const* t = d->type();
  if (is_object(d))
    t = t->ref();

  // Return a new expression.
  Expr* ret = new Decl_expr(t, d);
  locate(ret, loc);
  return ret;
}


// This deoes not require elaboration.
Expr*
Elaborator::elaborate(Decl_expr* e)
{
  return e;
}


namespace
{

// Used to require the conversion of a reference to a
// value. Essentially, this unwraps the reference if
// needed.
Expr*
require_value(Elaborator& elab, Expr* e)
{
  e = elab.elaborate(e);
  e = convert_to_value(e);
  return e;
}


// Used to require the conversion of an expression
// to a given type. This returns nullptr if the convesion
// fails.
Expr*
require_converted(Elaborator& elab, Expr* e, Type const* t)
{
  e = elab.elaborate(e);
  e = convert(e, t);
  return e;
}


// The operands of a binary arithmetic expression are
// converted to rvalues. The converted operands shall have
// type int. The result of an arithmetic expression is an
// rvalue with type int.
template<typename T>
Expr*
check_binary_arithmetic_expr(Elaborator& elab, T* e)
{
  Type const* z = get_integer_type();
  Expr* c1 = require_converted(elab, e->first, z);
  Expr* c2 = require_converted(elab, e->second, z);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'int'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'int'");

  // Rebuild the expression with the
  // converted operands.
  e->type_ = z;
  e->first = c1;
  e->second = c2;
  return e;
}


// The operands of a unary arithmetic expression are
// converted to rvalues. The converted operands shall
// have type int. The result of an arithmetic expression
// is an rvalue of type int.
template<typename T>
Expr*
check_unary_arithmetic_expr(Elaborator& elab, T* e)
{
  // Apply conversions
  Type const* z = get_integer_type();
  Expr* c = require_converted(elab, e->first, z);
  if (!c)
    throw Type_error({}, "operand cannot be converted to 'int'");

  // Rebuild the expression with the converted operands.
  e->type_ = z;
  e->first = c;
  return e;
}


} // namespace


Expr*
Elaborator::elaborate(Add_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Sub_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Mul_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Div_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Rem_expr* e)
{
  return check_binary_arithmetic_expr(*this, e);
}


//
Expr*
Elaborator::elaborate(Neg_expr* e)
{
  return check_unary_arithmetic_expr(*this, e);
}


Expr*
Elaborator::elaborate(Pos_expr* e)
{
  return check_unary_arithmetic_expr(*this, e);
}


namespace
{

// The operands of an equality expression are converted
// to rvalues. The operands shall have the same type. The
// result of an equality expression is an rvalue of type
// bool.
//
// TODO: Update equality comparison for new types.
Expr*
check_equality_expr(Elaborator& elab, Binary_expr* e)
{
  // Apply conversions.
  Type const* b = get_boolean_type();
  Expr* e1 = require_value(elab, e->first);
  Expr* e2 = require_value(elab, e->second);

  // Check types.
  if (e1->type() != e2->type())
    throw Type_error({}, "operands have different types");

  e->type_ = b;
  e->first = e1;
  e->second = e2;
  return e;
}

} // naespace


Expr*
Elaborator::elaborate(Eq_expr* e)
{
  return check_equality_expr(*this, e);
}


Expr*
Elaborator::elaborate(Ne_expr* e)
{
  return check_equality_expr(*this, e);
}



namespace
{

// The operands of an ordering expression are converted
// to rvalues. The operands shall have type int. The
// result of an equality expression is an rvalue of type
// bool.
//
// TODO: Update the ordering operands for new types.
Expr*
check_ordering_expr(Elaborator& elab, Binary_expr* e)
{
  // Apply conversions.
  Type const* z = get_integer_type();
  Type const* b = get_boolean_type();
  Expr* c1 = require_converted(elab, e->first, z);
  Expr* c2 = require_converted(elab, e->second, z);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'int'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'int'");

  // Rebuild the expression with the converted
  // operands.
  e->type_ = b;
  e->first = c1;
  e->second = c2;
  return e;
}


} // naespace


Expr*
Elaborator::elaborate(Lt_expr* e)
{
  return check_ordering_expr(*this, e);
}


Expr*
Elaborator::elaborate(Gt_expr* e)
{
  return check_ordering_expr(*this, e);
}


Expr*
Elaborator::elaborate(Le_expr* e)
{
  return check_ordering_expr(*this, e);
}


Expr*
Elaborator::elaborate(Ge_expr* e)
{
  return check_ordering_expr(*this, e);
}


namespace
{


// TODO: Document me!
Expr*
check_binary_logical_expr(Elaborator& elab, Binary_expr* e)
{
  // Apply conversions.
  Type const* b = get_boolean_type();
  Expr* c1 = require_converted(elab, e->first, b);
  Expr* c2 = require_converted(elab, e->second, b);
  if (!c1)
    throw Type_error({}, "left operand cannot be converted to 'bool'");
  if (!c2)
    throw Type_error({}, "right operand cannot be converted to 'bool'");

  // Rebuild the expression with the converted
  // operands.
  e->type_ = b;
  e->first = c1;
  e->second = c2;
  return e;
}


// TODO: Document me!
Expr*
check_unary_logical_expr(Elaborator& elab, Unary_expr* e)
{
  Type const* b = get_boolean_type();
  Expr* c = require_converted(elab, e->first, b);
  if (!c)
    throw Type_error({}, "operand cannot be converted to 'bool'");

  // Rebuild the expression with the converted
  // operand.
  e->type_ = b;
  e->first = c;
  return e;
}

} // namespace


Expr*
Elaborator::elaborate(And_expr* e)
{
  return check_binary_logical_expr(*this, e);
}


Expr*
Elaborator::elaborate(Or_expr* e)
{
  return check_binary_logical_expr(*this, e);
}


Expr*
Elaborator::elaborate(Not_expr* e)
{
  return check_unary_logical_expr(*this, e);
}


// Diagnose failures of argument conversion for
// function calls.
void
Elaborator::on_call_error(Expr_seq const& conv,
                          Expr_seq const& args,
                          Type_seq const& parms)
{
  if (args.size() < parms.size())
    throw Type_error({}, "too few arguments");
  if (parms.size() < args.size())
    throw Type_error({}, "too many arguments");

  for (std::size_t i = 0; i < parms.size(); ++i) {
    Expr const* c = conv[i];
    if (!c) {
      Expr const* a = args[i];
      Type const* p = parms[i];
      String s = format(
        "type mismatch in argument {} (expected {} but got {})",
        i + 1,
        *a->type(),
        *p);

      // FIXME: Don't fail on the first error.
      throw Type_error({}, s);
    }
  }
}


namespace
{

// Returns a dot-expr if e is of the form x.ovl.
// Otherwise, returns nullptr.
inline Dot_expr*
as_method_overload(Dot_expr* e)
{
  if (is<Overload_expr>(e->member()))
    return e;
  return nullptr;
}


inline Dot_expr*
as_method_overload(Expr* e)
{
  if (Dot_expr* dot = as<Dot_expr>(e))
    return as_method_overload(dot);
  return nullptr;
}


// Returns a dot-expr if e is of the form x.m or
// x.ovl. Otherwise, returns nullptr.
inline Dot_expr*
as_method(Expr* e)
{
  if (Dot_expr* dot = as<Dot_expr>(e)) {
    // x.y refers to a method.
    if (Method_expr* m = as<Method_expr>(dot))
      return m;

    // x.y refers to a field.
    if (is<Field_expr>(dot))
      return nullptr;

    // By elimination of cases, it must be a
    // method overload.
    lingo_assert(as_method_overload(dot));
    return dot;
  }
  return nullptr;
}


} // namespace


Expr*
Elaborator::call(Function_decl* d, Expr_seq const& args)
{
  Function_type const* t = d->type();

  // Perform argument conversion.
  Type_seq const& parms = t->parameter_types();
  Expr_seq conv = convert(args, parms);
  if (std::any_of(conv.begin(), conv.end(), [](Expr const* p) { return !p; }))
    return nullptr;

  // Update the expression with the return type
  // of the named function.
  Expr* ref = new Decl_expr(t, d);
  return new Call_expr(t->return_type(), ref, conv);
}


Expr*
Elaborator::resolve(Overload_expr* ovl, Expr_seq const& args)
{
  // Build a set of call expressions to the
  // declarations in the overload set.
  Expr_seq cands;
  Overload& decls = ovl->declarations();
  cands.reserve(decls.size());
  for (Decl* d : decls) {
    if (Expr* e = call(cast<Function_decl>(d), args))
      cands.push_back(e);
  }

  // FIXME: If the call is to a method, then write
  // out the method format for the call. Same as below.
  if (cands.empty()) {
    Location loc = locate(ovl);
    String msg = format("{}: no matching function for '{}'", loc, *ovl->name());
    std::cerr << msg << '\n';
    std::cerr << loc << ": candidates are:\n";
    for (Decl* d : decls) {
      std::cerr << format("{}: {}\n", locate(d), *d);
    }
    throw Type_error(locate(ovl), msg);
  }

  // TODO: Select the best candidate.
  if (cands.size() > 1) {
    String msg = format("call to function '{}' is ambiguous", *ovl->name());
  }

  return cands.front();
}



// Resolve a function call. The target of a function
// may be one of the following:
//
//    - a function f(args...)
//    - a function overload set ovl(args...)
//    - a method x.m(args...)
//    - a method overload set x.ovl(args...)
//
// In the case where the target is a method or
// member overload set of the form x.y(args...)
// the containing object x is added to the front
// of the argument list, and the funtion target
// is simply the method or overlaod set. That is,
// the following transformation is made:
//
//    x.y(args...) ~> y(x.args...)
//
// Let y be the new function target.
//
// If the function target is an overload set, select
// a function by overload resolution.
//
// TODO: If we support function objects by way of
// overloading the call operator, then the target
// could be an object or field of class type with
// one or more member call operators.
//
// TODO: Would it be better to differentiate
// function and method call and have those dealt
// with separately on the back end(s)?
//
// TODO: Support the lookup of member funtions using
// free-function notation:
//
//    f(x, args...) ~> x.f(args...)
//
// Applying this transformation might just entail
// the creation of a fake expression and its elaboration
// to resolve a method or overload set.
Expr*
Elaborator::elaborate(Call_expr* e)
{
  // Apply lvalue to rvalue conversion and ensure that
  // the target (ultimately) has function type.
  Expr* f = require_value(*this, e->target());
  if (!is_callable(f))
    throw Type_error({}, "object is not callable");

  // Elaborate the arguments (in place) prior to
  // conversion. Do it now so we don't re-elaborate
  // arguments during overload resolution.
  Expr_seq& args = e->arguments();
  for (Expr*& a : args)
    a = elaborate(a);

  // If the target is of the form x.m or x.ovl, insert x
  // into the argument list and update the function target.
  if (Dot_expr* dot = as_method(f)) {
      // Build the "this" argument.
      //Method_expr* m = dynamic_cast<Method_expr*>(dot);

      Expr* self = dot->container();

      //Expr* self = m->container();
      args.insert(args.begin(), self);

    // Adjust the function target.
    f = dot->member();
  }

  // Handle the case where f is an overload set.
  if (Overload_expr* ovl = as<Overload_expr>(f)) {
    return resolve(ovl, args);
  } else {
    // If it's not an overload set, it has function type.
    Function_type const* t = cast<Function_type>(f->type());

    // Perform argument conversion.
    Type_seq const& parms = t->parameter_types();
    Expr_seq conv = convert(args, parms);
    if (std::any_of(conv.begin(), conv.end(), [](Expr const* p) { return !p; }))
      on_call_error(conv, args, parms);

    // Update the expression with the return type
    // of the named function.
    e->type_ = t->return_type();
    e->first = f;
    e->second = conv;
  }

  // Guarantee that f is an expression that refers
  // to a declaration.
  lingo_assert(is<Decl_expr>(f) &&
               is<Function_decl>(cast<Decl_expr>(f)->declaration()));

  // Update the call expression before returning.
  return e;
}


namespace
{

// Search the base classes for the given field.
//
// TODO: Support multiple base classes.
void
get_path(Record_decl* r, Field_decl* f, Field_path& p)
{
  // Search the record for the given fields.
  Decl_seq const& fs = r->fields();
  auto iter = std::find(fs.begin(), fs.end(), f);
  if (iter != fs.end()) {
    // Compute the offset adjustment for this member.
    // A virtual table reference counts as a subobject, and 
    // so does a base class sub-object.
    int a = 0;
    if (r->vref())
      ++a;
    if (r->base())
      ++a;
    p.push_back(std::distance(fs.begin(), iter) + a);
    return;
  }

  // Recursively search the base class.
  if (r->base()) {
    p.push_back(0);
    get_path(r->base()->declaration(), f, p);  
  }
}


// Construct a sequence of indexes through the record and
// up to the given field. The resulting path shall be
// a non-empty sequence of indexes.
Field_path
get_path(Record_decl* r, Field_decl* f)
{
  Field_path p;
  get_path(r, f, p);
  lingo_assert(!p.empty());
  return p;
}

//Method_path
//get_path(Record_decl* r, Method_decl* m){
//  Method_path p;
//  get_path(r, m, p);
//  lingo_assert(!p.empty());
//  return p;
//}


} // namespace


// TODO: Document the semantics of member access.
Expr*
Elaborator::elaborate(Dot_expr* e)
{
  Expr* e1 = elaborate(e->container());
  if (!is<Reference_type>(e1->type())) {
    std::stringstream ss;
    ss << "cannot access a member of a non-object";
    throw Type_error({}, ss.str());
  }

  // Get the non-reference type of the outer object 
  // so we can perform qualified lookup.
  //
  // TODO: If we support modules, we would need to allow
  // for different kinds of scopes here.
  Record_type const* t1 = as<Record_type>(e1->type()->nonref());
  if (!t1) {
    std::stringstream ss;
    ss << "object does not have record type";
    throw Type_error({}, ss.str());
  }
  Scope* s = t1->declaration()->scope();

  // We expect the member to be an unresolved id expression.
  // If it isn't, there's not much we can do with it.
  //
  // TODO: Maybe allow a literal value in this position
  // to support tuple access?
  //
  //    t.0 -- get the first tuple element?
  Expr* e2 = e->member();
  if (!is<Id_expr>(e2)) {
    String msg = format("invalid member '{}'", *e2);
    throw Type_error(locate(e2), msg);
  }
  Id_expr* id = cast<Id_expr>(e2);

  // Perform qualified lookup on the member.
  Overload* ovl = qualified_lookup(s, id->symbol());
  if (!ovl) {
    String msg = format("no member matching '{}'", *id);
    throw Lookup_error(locate(id), msg);
  }

  // If we get a single declaration, return a corresponding
  // expression.
  if (ovl->size() == 1) {
    Decl*d = ovl->front();
    e2 = new Decl_expr(d->type(), d);
    if (Field_decl* f = as<Field_decl>(d)) {
      Type const* t2 = e2->type()->ref();
      Field_path p = get_path(t1->declaration(), f);
      return new Field_expr(t2, e1, e2, f, p);
    }
    if (Method_decl* m = as<Method_decl>(d)) {
      return new Method_expr(e1, e2, m);
    }
  }

  // Otherwise, if the name resolves to a set of declarations,
  // then the declaration is still unresolved. Update the
  // expression with the overload set and defer until we find
  // a function call.
  else {
    e->first = e1;
    e->second = new Overload_expr(ovl);
    return e;
  }

  // Otherwise, this is an error.
  std::stringstream ss;
  ss << "invalid member reference";
  throw Type_error({}, ss.str());
}


Expr*
Elaborator::elaborate(Field_expr* e)
{
  return e;
}


Expr*
Elaborator::elaborate(Method_expr* e)
{
  return e;
}


// In the expression e1[e2], e1 shall be an object of
// array type T[N] (for some N) or block type T[]. The
// expression e2 shall be an integer value. The result
// type of the expressions is ref T.
//
// Note that e1 is not converted to a value, and in fact
// *must* be a reference to an object. Converting to a
// value will prevent me from creating element pointers
// in code gen, because we need the base pointer from
// which to compute offsets.
Expr*
Elaborator::elaborate(Index_expr* e)
{
  Expr* e1 = elaborate(e->first);
  if (!is<Reference_type>(e1->type())) {
    std::stringstream ss;
    ss << "cannot index into a value";
    throw Type_error({}, ss.str());
  }

  // Get the non-reference type of the array.
  //
  // FIXME: Does this require a value transformation?
  // We don't (yet?) have array literals, so I generally
  // expect that this *must* be a reference to an array.
  //
  // TODO: Allow block type.
  Array_type const* t = as<Array_type>(e1->type()->nonref());
  if (!t) {
    std::stringstream ss;
    ss << "object does not have array type";
    throw Type_error({}, ss.str());
  }

  // The index shall be an integer value.
  Expr* e2 = require_converted(*this, e->second, get_integer_type());

  // The result type shall be ref T.
  e->type_ = get_reference_type(t->type());
  e->first = e1;
  e->second = e2;

  return e;
}


// NOTE: Conversions are created after their source
// expressions  have been elaborated. No action is
// required.

Expr*
Elaborator::elaborate(Value_conv* e)
{
  return e;
}


Expr*
Elaborator::elaborate(Block_conv* e)
{
  return e;
}

Expr*
Elaborator::elaborate(Base_conv* e)
{
  return e;
}

Expr*
Elaborator::elaborate(Promote_conv* e)
{
  return e;
}


// TODO: I probably need to elaborate the type.
Expr*
Elaborator::elaborate(Default_init* e)
{
  e->type_ = elaborate(e->type_);
  return e;
}


Expr*
Elaborator::elaborate(Trivial_init* e)
{
  e->type_ = elaborate(e->type_);
  return e;
}


Expr*
Elaborator::elaborate(Copy_init* e)
{
  // Elaborate the type.
  e->type_ = elaborate(e->type_);

  // If copying into a reference, we're actually
  // performing reference initialization. Create
  // a new node and elaborate it.
  if (is<Reference_type>(e->type())) {
    Reference_init* init = new Reference_init(e->type(), e->value());
    return elaborate(init);
  }

  // Otherwise, this actually a copy.
  //
  // TOOD: This should perform a lookup to find a
  // function that implements copies. It could be
  // bitwise copy, a memberwise copy, or a copy
  // constructor.

  // Convert the value to the resulting type.
  Expr* c = require_converted(*this, e->first, e->type_);
  if (!c) {
    std::stringstream ss;
    ss << "type mismatch in copy initializer (expected "
       << *e->type() << " but got " << *e->value()->type() << ')';
    throw Type_error({}, ss.str());
  }
  e->first = c;

  return e;
}


// Note that this is only ever called from the
// elaborator for copy initialization. The type must
// already be elaborated.
Expr*
Elaborator::elaborate(Reference_init* e)
{
  Expr* obj = elaborate(e->object());

  // A reference can only be bound to an object.
  if (!is<Reference_type>(obj->type())) {
    throw Type_error({}, "binding reference to temporary");
  }

  // TODO: Allow t2 to be derived from t1.
  //
  //    struct B { };
  //    struct D : B { };
  //
  //    var obj : D;
  //    var ref : B& = obj;
  //
  // TODO: Allow t2 to be less cv qualified than t1.
  // That would allow bindings to constants:
  //
  //    var T x;
  //    var T const& c = x;
  Type const* t1 = e->type();
  Type const* t2 = obj->type();
  if (t1->nonref() != t2->nonref()) {
    std::stringstream ss;
    ss << "binding reference to an object of a different type"
       << "(expected " << *t1 << " but got " << *t2 << ')';
    throw Type_error({}, ss.str());
  }

  // Update the expression.
  e->first = obj;

  return e;
}


// -------------------------------------------------------------------------- //
// Elaboration of declarations

// Elaborate a declaration. This returns true if
// elaboration succeeds and false otherwise.
Decl*
Elaborator::elaborate(Decl* d)
{
  struct Fn
  {
    Elaborator& elab;
    Decl* operator()(Variable_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Function_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Parameter_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Record_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Field_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Method_decl* d) const { return elab.elaborate(d); }
    Decl* operator()(Module_decl* d) const { return elab.elaborate(d); }
  };

  return apply(d, Fn{*this});
}


// The type of the initializer shall match the declared type
// of the variable.
//
// The variable is declared prior to the elaboration of its
// initializer.
Decl*
Elaborator::elaborate(Variable_decl* d)
{
  d->type_ = elaborate_type(d->type_);

  // Declare the variable.
  declare(d);

  // Elaborate the initializer. Note that the initializers
  // type must be the same as that of the declaration.
  d->init_ = elaborate(d->init());

  // Annotate the initializer with the declared
  // object.
  //
  // TODO: This will probably be an expression in
  // the future.
  cast<Init>(d->init())->decl_ = d;

  return d;
}


// The types of return expressions shall match the declared
// return type of the function.
Decl*
Elaborator::elaborate(Function_decl* d)
{
  throw Type_error(locate(d), "function declaration in block scope");
}


// Elaborate a parameter declaration. This simply declares
// the parameter in the current scope.
Decl*
Elaborator::elaborate(Parameter_decl* d)
{
  d->type_ = elaborate_type(d->type_);
  declare(d);

  Function_decl* fn = stack.function();

  // Check for virtual parameters. A parameter can only be
  // declared virtual if t has polymorphic type (or is a reference
  // to an object of polymorphic type).
  if (d->is_virtual()) {
    Type const* t0 = d->type()->nonref();
    if (!is<Record_type>(t0))
      throw Type_error(locate(d), "type of virtual parameter is not a record type");
    Record_type const* t1 = cast<Record_type>(t0);
    Record_decl const* rec = t1->declaration();
    if (!rec->is_polymorphic())
      throw Type_error(locate(d), "type of virtual parameter is not polymorphic");

    // Mark the function as being virtual.
    fn->spec_ |= virtual_spec;

    // Save virtual parameter.
    //
    // TODO: What are we actually going to do with
    // this thing?
    if (!fn->vparms_)
      fn->vparms_ = new Decl_seq {d};
    else
      fn->vparms_->push_back(d);
  }

  return d;
}


// FIXME: Allow records in block scope?
Decl*
Elaborator::elaborate(Record_decl* d)
{
  throw Type_error(locate(d), "record declaration in block scope");
}


// There is no single pass elaborator for fields.
Decl*
Elaborator::elaborate(Field_decl* d)
{
  lingo_unreachable();
}


// There is no single pass elaborator for methods.
Decl*
Elaborator::elaborate(Method_decl* d)
{
  lingo_unreachable();
}


// Elaborate the module. Modules use a two phase
// lookup mechanism.
Decl*
Elaborator::elaborate(Module_decl* m)
{
  Scope_sentinel scope(*this, m);
  for (Decl*& d : m->decls_)
    d = elaborate_decl(d);
  for (Decl*& d : m->decls_)
    d = elaborate_def(d);
  return m;
}


// -------------------------------------------------------------------------- //
// Elaboration of declarations (but not definitions)

namespace
{

// Defined here because of the member template.
struct Elab_decl_fn
{
  Elaborator& elab;
  Decl* operator()(Variable_decl* d) const { return elab.elaborate_decl(d); }
  Decl* operator()(Function_decl* d) const { return elab.elaborate_decl(d); }
  Decl* operator()(Parameter_decl* d) const { return elab.elaborate_decl(d); }
  Decl* operator()(Record_decl* d) const { return elab.elaborate_decl(d); }
  Decl* operator()(Field_decl* d) const { return elab.elaborate_decl(d); }
  Decl* operator()(Method_decl* d) const { return elab.elaborate_decl(d); }
  Decl* operator()(Module_decl* d) const { return elab.elaborate_decl(d); }
};


} // namespace

Decl*
Elaborator::elaborate_decl(Decl* d)
{
  return apply(d, Elab_decl_fn{*this});
}


Decl*
Elaborator::elaborate_decl(Variable_decl* d)
{
  d->type_ = elaborate_type(d->type_);
  declare(d);
  return d;
}


Decl*
Elaborator::elaborate_decl(Function_decl* d)
{
  d->type_ = elaborate_type(d->type_);
  declare(d);

  // Remember if we've seen a function named main().
  //
  // FIXME: This seems dumb. Is there a better way
  // of handling the discovery and elaboration of
  // main?
  if (d->name() == syms.get("main")) {
    main = d;

    // Ensure that main has foreign linkage.
    d->spec_ |= foreign_spec;

    // TODO: Check that main conforms to the
    // expected return type and arguments.
  }

  return d;
}


Decl*
Elaborator::elaborate_decl(Parameter_decl* d)
{
  lingo_unreachable();
}


Decl*
Elaborator::elaborate_decl(Field_decl* d)
{
  d->type_ = elaborate_type(d->type_);
  declare(d);
  return d;
}


Decl*
Elaborator::elaborate_decl(Record_decl* d)
{
  declare(d);
  return d;
}


namespace
{

// Returns true if m1 is an override of m2. This is the
// case when m1 has the same name and type as m2.
//
// TODO: Allow for covariant return types.
//
// TODO: Consider allowing contravariant argument types?
bool
is_override(Method_decl const* m1, Method_decl const* m2)
{
  // Different name? Not an overrider.
  if (m1->name() != m2->name())
    return false;

  // Different return types? Not an overrider.
  //
  // TODO: Support covariant return types here.
  Function_type const* t1 = m1->type();
  Function_type const* t2 = m2->type();
  if (t1->return_type() != t2->return_type())
    return false;

  // Compare all parameter types except the implicit this
  // parameter. Those differ by definition (m1's this is
  // derived from m2's this).
  Type_seq const& p1 = m1->type()->parameter_types();
  Type_seq const& p2 = m2->type()->parameter_types();
  if (p1.size() != p2.size())
    return false;
  for (std::size_t i = 1; i < p1.size(); ++i) {
    // TODO: Support covariant return types here.
    if (p1[i] != p2[i])
      return false;
  }
  return true;
}


// Find the method in d that m overrides anmd return the
// offset in the virtual table. If the record is not polymorphic
// then this cannot be an overrider.
int
find_override(Record_decl const* d, Method_decl const* m)
{
  Decl_seq const& ms = *d->vtable();
  auto iter = std::find_if(ms.begin(), ms.end(), [m](Decl const* m2) {
    return is_override(m, cast<Method_decl>(m2));
  });
  if (iter != ms.end())
    return std::distance(ms.begin(), iter);
  else
    return -1;
}


} // namespace


// Insert the implicit this parameter and adjust the
// type of the declaration.
Decl*
Elaborator::elaborate_decl(Method_decl* d)
{
  Record_decl* rec = stack.record();
  Decl_seq* vtable = rec->vtable();

  // Generate the type of the implicit this parameter.
  //
  // TODO: Handle constant references.
  Type const* type = get_reference_type(get_record_type(rec));

  // Re-build the function type.
  //
  // TODO: Factor this out as an operation on a method.
  Function_type const* ft = cast<Function_type>(elaborate(d->type()));
  Type_seq pt = ft->parameter_types();
  pt.insert(pt.begin(), type);
  Type const* rt = ft->return_type();
  Type const* mt = get_function_type(pt, rt);
  d->type_ = mt;

  // Actually build the implicit this parameter and add it
  // to the front of the list of parameters.
  Symbol const* name = syms.get("this");
  Parameter_decl* self = new Parameter_decl(name, type);
  d->parms_.insert(d->parms_.begin(), self);


  // Propagate virtual/abstract specifiers to the class.
  // Build a new virtual table as needed.
  if (d->is_virtual())
    rec->spec_ |= virtual_spec;
  if (d->is_abstract())
    rec->spec_ |= abstract_spec;
  if (rec->is_polymorphic() && !vtable)
    rec->vtbl_ = vtable = new Decl_seq();

  // This function may be an override of a previous
  // virtual function -- even if it wasn't declared as
  // such. Propagate flags from an overrider if there
  // was one.
  if (vtable) {
    int ent = find_override(rec, d);
    if (ent >= 0) {
      // Update the method (and class) with the state
      // from the overrider.
      //
      // TODO: Actually save the overriden function with
      // the current declaration?
      //
      // TODO: Can I declare an abstract overrider?
      Method_decl const* m = cast<Method_decl>((*vtable)[ent]);
      if (m->is_polymorphic()) {
        d->spec_ |= virtual_spec;
        rec->spec_ |= virtual_spec;
      }

      // Overwrite the overrider in the vtable.
      (*vtable)[ent] = d;
    } else {
      // Extend the virtual table.
      ent = vtable->size();
      vtable->push_back(d);
    }
    d->vtent_ = ent;
  }

  // Note that we don't need to elaborate or declare
  // the funciton parameters because they're only visible
  // within the function body (see the def elaborator for
  // function definitions).

  // Now declare the method.
  declare(d);

  return d;
}


// Since modules aren't nested, we should never get here.
Decl*
Elaborator::elaborate_decl(Module_decl* d)
{
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Elaboration of definitions

namespace
{

// Defined here because of the member template.
struct Elab_def_fn
{
  Elaborator& elab;
  Decl* operator()(Variable_decl* d) const { return elab.elaborate_def(d); }
  Decl* operator()(Function_decl* d) const { return elab.elaborate_def(d); }
  Decl* operator()(Parameter_decl* d) const { return elab.elaborate_def(d); }
  Decl* operator()(Record_decl* d) const { return elab.elaborate_def(d); }
  Decl* operator()(Field_decl* d) const { return elab.elaborate_def(d); }
  Decl* operator()(Method_decl* d) const { return elab.elaborate_def(d); }
  Decl* operator()(Module_decl* d) const { return elab.elaborate_def(d); }
};


} // namespace


Decl*
Elaborator::elaborate_def(Decl* d)
{
  return apply(d, Elab_def_fn{*this});
}


Decl*
Elaborator::elaborate_def(Variable_decl* d)
{
  // Elaborate the initializer.
  d->init_ = elaborate(d->init());

  // Annotate the initializer with the declared
  // object.
  //
  // FIXME: This needs to be rethought.
  cast<Init>(d->init())->decl_ = d;
  return d;
}


Decl*
Elaborator::elaborate_def(Function_decl* d)
{
  // Enter the function scope and declare all of
  // the parameters (by way of elaboration).
  //
  // Note that this modifies the original parameters.
  Scope_sentinel scope(*this, d);
  for (Decl*& p : d->parms_)
    p = elaborate(p);

  // Check the body of the function, if present.
  if (d->body())
    d->body_ = elaborate(d->body());

  // TODO: Are we actually checking returns match
  // the return type?

  // TODO: Build a control flow graph and ensure that
  // every branch returns a value.

  return d;
}


Decl*
Elaborator::elaborate_def(Parameter_decl* d)
{
  lingo_unreachable();
}


// Returns true if we are currently defining the declaration d.
// That is te case when the declaraiton appears in the decl
// stack.
bool
Elaborator::is_defining(Decl const* d) const
{
  return count(defining.begin(), defining.end(), d);
}


// FIXME: If the type of a member variable requires the
// definition of a user-defined type, then we need to
// recursively elaborate that. However, we need to be
// careful that we don't generate cycles.
Decl*
Elaborator::elaborate_def(Record_decl* d)
{
  // If the declaration has already been declared,
  // then don't re-elaborate it.
  if (defined.count(d))
    return d;

  // Prevent recursive type definitions.
  if (is_defining(d)) {
    std::cerr << format("cyclic definition of '{}'\n", *d->name());
    for (auto iter = defining.rbegin(); iter != defining.rend(); ++iter) {
      if (*iter == d)
        break;
      std::cerr << format("  referenced in the definition of '{}'\n", *(*iter)->name());
    }
    throw Type_error(locate(d), format("cyclic definition of '{}'", *d->name()));
  }
  Defining_sentinel def(*this, d);

  // Elaborate base class. 
  if (d->base_)
    d->base_ = elaborate(d->base_);

  // If the base class is polymorphic, then so is the 
  // derived class. Propagate the virtual table to this 
  // class.
  //
  // FIXME: A derived class is abstract only if it fails
  // to provide overriders for each all abstract methods.
  // Think of a better way for this to work.
  Record_decl const* base = d->base_declaration();
  if (base) {
    if (base->is_virtual())
      d->spec_ |= virtual_spec;
    if (base->is_abstract())
      d->spec_ |= abstract_spec;
    if (base->is_polymorphic())
      d->vtbl_ = new Decl_seq(*base->vtable());
  }

  // Elaborate member declarations, fields first.
  //
  // TODO: What are the lookup rules for default
  // member initializers. If we do this:
  //
  //    struct S {
  //      x : int = 1;
  //      y : int = x + 2; // Seems resonable.
  //
  //      a : int = b - 1; // OK?
  //      b : int = 0;
  //      // Making this okay could impose an alternative
  //      // initialization order.
  //
  //      g : int = f();   // OK?
  //      def f() -> int { ... }
  //      // What if f() refers to an uninitialized fiedl?
  //    }
  Scope_sentinel scope(*this, d->scope());
  for (Decl*& f : d->fields_)
    f = elaborate_decl(f);
  for (Decl*& m : d->members_)
    m = elaborate_decl(m);

  // Elaborate member definitions. See comments
  // above about handling member defintions.
  for (Decl*& m : d->members_)
    m = elaborate_def(m);

  // Determine if we need a vtable reference. This is the case 
  // when:
  //    - there is no base class or
  //    - the base is not polymorphic
  //
  // TODO: We may need to perform this transformation
  // before elaborating any fields. It depends on whether
  // or not we allow a member's type to refer to member
  // variables (a la decltype).
  //
  // TODO: For multiple base classes, we probably want
  // multiple vtable references (one for each base).
  if (d->is_polymorphic()) {
    if (!base || !base->is_polymorphic()) {
      Symbol const* n = syms.get("vref");
      Type const* p = get_reference_type(get_character_type());
      d->vref_ = new Field_decl(n, p);
    }
  }

  defined.insert(d);
  return d;
}


// Nothing to do here now...
Decl*
Elaborator::elaborate_def(Field_decl* d)
{
  return d;
}


// Elaborate the method as a function. Note that we pre-declare
// the implicit "this" parameter during the first pass.
Decl*
Elaborator::elaborate_def(Method_decl* d)
{
  elaborate_def(cast<Function_decl>(d));
  return d;
}


Decl*
Elaborator::elaborate_def(Module_decl* d)
{
  // We should never get here.
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Elaboration of statements

// Elaborate a statement. This returns true if elaboration
// succeeds and false otherwise.
Stmt*
Elaborator::elaborate(Stmt* s)
{
  struct Fn
  {
    Elaborator& elab;

    Stmt* operator()(Empty_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Block_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Assign_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Return_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(If_then_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(If_else_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(While_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Break_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Continue_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Expression_stmt* d) const { return elab.elaborate(d); }
    Stmt* operator()(Declaration_stmt* d) const { return elab.elaborate(d); }
  };

  return apply(s, Fn{*this});
}


Stmt*
Elaborator::elaborate(Empty_stmt* s)
{
  return s;
}


Stmt*
Elaborator::elaborate(Block_stmt* s)
{
  Scope_sentinel scope = *this;
  for (Stmt*& s1 : s->first)
    s1 = elaborate(s1);
  return s;
}


// In an assignment expression, the left operand shall
// refer to a mutable object. The types of the left and
// right operands shall match.
//
// TODO: If we have const types, then we'd have to add this
// checking.
Stmt*
Elaborator::elaborate(Assign_stmt* s)
{
  // FIXME: Write a better predicate?
  Expr* lhs = elaborate(s->object());
  if (!is<Reference_type>(lhs->type()))
    throw Type_error({}, "assignment to rvalue");

  // Apply rvalue conversion to the value and update the
  // expression.
  Expr *rhs = require_value(*this, s->second);

  // The types shall match. Compare t1 using the non-reference
  // type of the object.
  Type const* t1 = lhs->type()->nonref();
  Type const* t2 = rhs->type();
  if (t1 != t2)
    throw Type_error({}, "assignment to an object of a different type");

  s->first = lhs;
  s->second = rhs;
  return s;
}


// The type of the returned expression shall match the declared
// return type of the enclosing function.
//
// TODO: Implement me.
Stmt*
Elaborator::elaborate(Return_stmt* s)
{
  Function_decl* fn = stack.function();
  Type const* t = fn->return_type();

  // Check that the return type matches the returned value.
  Expr* e = elaborate(s->value());
  Expr* c = convert(e, t);
  if (!c) {
    std::stringstream ss;
    ss << "return type mismatch (expected "
       << *t << " but got " << *s->value()->type() << ")";
    throw std::runtime_error(ss.str());
  }

  s->first = c;
  return s;
}


// The condition must must be a boolean expression.
Stmt*
Elaborator::elaborate(If_then_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "if condition does not have type 'bool'");
  Stmt* b = elaborate(s->body());

  s->first = c;
  s->second = b;
  return s;
}


// The condition must must be a boolean expression.
Stmt*
Elaborator::elaborate(If_else_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "if condition does not have type 'bool'");
  Stmt* t = elaborate(s->true_branch());
  Stmt* f = elaborate(s->false_branch());

  s->first = c;
  s->second = t;
  s->third = f;
  return s;
}


Stmt*
Elaborator::elaborate(While_stmt* s)
{
  Expr* c = require_converted(*this, s->first, get_boolean_type());
  if (!c)
    throw Type_error({}, "loop condition does not have type 'bool'");
  Stmt* b = elaborate(s->body());

  s->first = c;
  s->second = b;
  return s;
}


Stmt*
Elaborator::elaborate(Break_stmt* s)
{
  // TODO: Verify that a break occurs within an
  // appropriate context.
  return s;
}


Stmt*
Elaborator::elaborate(Continue_stmt* s)
{
  // TODO: Verify that a continue occurs within an
  // appropriate context.
  return s;
}


Stmt*
Elaborator::elaborate(Expression_stmt* s)
{
  s->first = elaborate(s->expression());
  return s;
}


Stmt*
Elaborator::elaborate(Declaration_stmt* s)
{
  s->first = elaborate(s->declaration());
  return s;
}
