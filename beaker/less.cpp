// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "less.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "value.hpp"

#include <algorithm>
#include <typeindex>


// -------------------------------------------------------------------------- //
// Generic comparisons


template<typename T>
inline bool
is_less(std::vector<T> const& a, std::vector<T> const& b)
{
  auto cmp = [](T const& x, T const& y) { return is_less(x, y); };
  return std::lexicographical_compare(a.begin(), a.end(),
                                      b.begin(), b.end(), cmp);
}


template<typename T>
typename std::enable_if<is_nullary_node<T>(), bool>::type
is_less(T const* a, T const* b)
{
  return false;
}


template<typename T>
typename std::enable_if<is_unary_node<T>(), bool>::type
is_less(T const* a, T const* b)
{
  return is_less(a->first, b->first);
}


template<typename T>
typename std::enable_if<is_binary_node<T>(), bool>::type
is_less(T const* a, T const* b)
{
  if (is_less(a->first, b->first))
    return true;
  if (is_less(b->first, a->first))
    return false;
  return is_less(a->second, b->second);
}


template<typename T>
typename std::enable_if<is_ternary_node<T>(), bool>::type
is_less(T const* a, T const* b)
{
  if (is_less(a->first, b->first))
    return true;
  if (is_less(b->first, a->first))
    return false;
  if (is_less(a->second, b->second))
    return true;
  if (is_less(b->second, a->second))
    return false;
  return is_less(a->third, b->third);
}


// -------------------------------------------------------------------------- //
// Comparison of types


inline bool
is_less(Id_type const* a, Id_type const* b)
{
  std::less<void const*> cmp;
  return cmp(a->symbol(), b->symbol());
}


inline bool
is_less(Record_type const* a, Record_type const* b)
{
  std::less<void const*> cmp;
  return cmp(a->declaration(), b->declaration());
}


bool
is_less(Type const* a, Type const* b)
{
  struct Fn
  {
    Type const* b;

    bool operator()(Id_type const* a) { return is_less(a, cast<Id_type>(b)); }
    bool operator()(Boolean_type const* a) { return false; }
    bool operator()(Character_type const* a) { return false; }
    bool operator()(Integer_type const* a) { return false; }
    bool operator()(Function_type const* a) { return is_less(a, cast<Function_type>(b)); }
    bool operator()(Array_type const* a) { return is_less(a, cast<Array_type>(b)); }
    bool operator()(Block_type const* a) { return is_less(a, cast<Block_type>(b)); }
    bool operator()(Reference_type const* a) { return is_less(a, cast<Reference_type>(b)); }
    bool operator()(Record_type const* a) { return is_less(a, cast<Record_type>(b)); }
  };

  std::type_index t1 = typeid(*a);
  std::type_index t2 = typeid(*b);
  if (t1 < t2)
    return true;
  if (t2 < t1)
    return false;
  return apply(a, Fn{b});
}


// -------------------------------------------------------------------------- //
// Comparison of values


// FIXME: Use a visitor for values. Also, push this into
// a header file somewhere.
bool
is_less(Value const& a, Value const& b)
{
  if (a.kind() < b.kind())
    return true;
  if (b.kind() < a.kind())
    return false;

  switch (a.kind()) {
    case error_value:
      return false;

    case integer_value:
      return a.get_integer() < b.get_integer();

    case function_value: {
      std::less<void const*> cmp;
      return cmp(a.get_function(), b.get_function());
    }

    case reference_value:
      return is_less(*a.get_reference(), *b.get_reference());

    case array_value:
    case tuple_value:
      // FIXME: Implement me!
      return false;
  }
  throw std::runtime_error("unhandled value");
}


// -------------------------------------------------------------------------- //
// Comparison of expressions


inline bool
is_less(Literal_expr const* a, Literal_expr const* b)
{
  return is_less(a->value(), b->value());
}


// FIXME: Actually implement this!
bool
is_less(Expr const* a, Expr const* b)
{
  struct Fn
  {
    Expr const* b;

    bool operator()(Literal_expr const* a) { return is_less(a, cast<Literal_expr>(b)); }
    bool operator()(Id_expr const* a) { throw std::runtime_error("comparing Id_expr"); }
    bool operator()(Add_expr const* a) { throw std::runtime_error("comparing Add_expr"); }
    bool operator()(Sub_expr const* a) { throw std::runtime_error("comparing Sub_expr"); }
    bool operator()(Mul_expr const* a) { throw std::runtime_error("comparing Mul_expr"); }
    bool operator()(Div_expr const* a) { throw std::runtime_error("comparing Div_expr"); }
    bool operator()(Rem_expr const* a) { throw std::runtime_error("comparing Rem_expr"); }
    bool operator()(Neg_expr const* a) { throw std::runtime_error("comparing Neg_expr"); }
    bool operator()(Pos_expr const* a) { throw std::runtime_error("comparing Pos_expr"); }
    bool operator()(Eq_expr const* a) { throw std::runtime_error("comparing Eq_expr"); }
    bool operator()(Ne_expr const* a) { throw std::runtime_error("comparing Ne_expr"); }
    bool operator()(Lt_expr const* a) { throw std::runtime_error("comparing Lt_expr"); }
    bool operator()(Gt_expr const* a) { throw std::runtime_error("comparing Gt_expr"); }
    bool operator()(Le_expr const* a) { throw std::runtime_error("comparing Le_expr"); }
    bool operator()(Ge_expr const* a) { throw std::runtime_error("comparing Ge_expr"); }
    bool operator()(And_expr const* a) { throw std::runtime_error("comparing And_expr"); }
    bool operator()(Or_expr const* a) { throw std::runtime_error("comparing Or_expr"); }
    bool operator()(Not_expr const* a) { throw std::runtime_error("comparing Not_expr"); }
    bool operator()(Call_expr const* a) { throw std::runtime_error("comparing Call_expr"); }
    bool operator()(Member_expr const* a) { throw std::runtime_error("comparing Member_expr"); }
    bool operator()(Index_expr const* a) { throw std::runtime_error("comparing Index_expr"); }
    bool operator()(Value_conv const* a) { throw std::runtime_error("comparing Value_conv"); }
    bool operator()(Block_conv const* a) { throw std::runtime_error("comparing Block_conv"); }
    bool operator()(Default_init const* a) { throw std::runtime_error("comparing Default_init"); }
    bool operator()(Copy_init const* a) { throw std::runtime_error("comparing Copy_init"); }
  };

  std::type_index t1 = typeid(*a);
  std::type_index t2 = typeid(*b);
  if (t1 < t2)
    return true;
  if (t2 < t1)
    return false;
  return apply(a, Fn{b});
}
