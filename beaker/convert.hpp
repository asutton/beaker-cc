
#ifndef CONVERT_HPP
#define CONVERT_HPP

// The convert module defines various implicit conversion
// routines used by the language.

#include "prelude.hpp"


bool is_lvalue(Expr const*);
bool is_rvalue(Expr const*);

Expr* convert(Expr*, Type const*);
Expr* lvalue_to_rvalue(Expr*, Type const*);
Expr* lvalue_to_rvalue(Expr*);


#endif
