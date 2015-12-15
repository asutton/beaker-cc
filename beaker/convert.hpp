// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef CONVERT_HPP
#define CONVERT_HPP

// The convert module defines various implicit conversion
// routines used by the language.

#include <beaker/prelude.hpp>


class Elaborator;

Expr*    convert(Expr*, Type const*);
Expr_seq convert(Expr_seq const&, Type_seq const&);

Expr* convert_to_value(Expr*);
Type const* get_promotion_target(Expr*, Expr*);
Type const* get_promotion_target(Expr*);
Expr* promote(Expr*, Type const*);

#endif
