
#ifndef CONVERT_HPP
#define CONVERT_HPP

// The convert module defines various implicit conversion
// routines used by the language.

#include "prelude.hpp"


Expr* convert(Expr*, Type const*);
Expr* convert_to_value(Expr*);


#endif
