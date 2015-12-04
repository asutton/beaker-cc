
#ifndef CONVERT_HPP
#define CONVERT_HPP

// The convert module defines various implicit conversion
// routines used by the language.

#include "prelude.hpp"


class Elaborator;

Expr*    convert(Expr*, Type const*);
Expr_seq convert(Expr_seq const&, Type_seq const&);

Expr* convert_to_value(Expr*);


#endif
