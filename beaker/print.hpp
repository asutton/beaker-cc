// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PRINT_HPP
#define BEAKER_PRINT_HPP

#include "prelude.hpp"

#include <iosfwd>

std::ostream& operator<<(std::ostream&, Type const&);
std::ostream& operator<<(std::ostream&, Id_type const&);
std::ostream& operator<<(std::ostream&, Boolean_type const&);
std::ostream& operator<<(std::ostream&, Integer_type const&);
std::ostream& operator<<(std::ostream&, Function_type const&);
std::ostream& operator<<(std::ostream&, Block_type const&);
std::ostream& operator<<(std::ostream&, Array_type const&);
std::ostream& operator<<(std::ostream&, Reference_type const&);
std::ostream& operator<<(std::ostream&, Record_type const&);

std::ostream& operator<<(std::ostream&, Expr const&);
std::ostream& operator<<(std::ostream&, Literal_expr const&);
std::ostream& operator<<(std::ostream&, Id_expr const&);
std::ostream& operator<<(std::ostream&, Add_expr const&);
std::ostream& operator<<(std::ostream&, Sub_expr const&);
std::ostream& operator<<(std::ostream&, Mul_expr const&);
std::ostream& operator<<(std::ostream&, Div_expr const&);
std::ostream& operator<<(std::ostream&, Rem_expr const&);
std::ostream& operator<<(std::ostream&, Neg_expr const&);
std::ostream& operator<<(std::ostream&, Pos_expr const&);
std::ostream& operator<<(std::ostream&, Eq_expr const&);
std::ostream& operator<<(std::ostream&, Ne_expr const&);
std::ostream& operator<<(std::ostream&, Lt_expr const&);
std::ostream& operator<<(std::ostream&, Gt_expr const&);
std::ostream& operator<<(std::ostream&, Le_expr const&);
std::ostream& operator<<(std::ostream&, Ge_expr const&);
std::ostream& operator<<(std::ostream&, And_expr const&);
std::ostream& operator<<(std::ostream&, Or_expr const&);
std::ostream& operator<<(std::ostream&, Not_expr const&);
std::ostream& operator<<(std::ostream&, Call_expr const&);
std::ostream& operator<<(std::ostream&, Member_expr const&);
std::ostream& operator<<(std::ostream&, Index_expr const&);
std::ostream& operator<<(std::ostream&, Value_conv const&);
std::ostream& operator<<(std::ostream&, Block_conv const&);
std::ostream& operator<<(std::ostream&, Default_init const&);
std::ostream& operator<<(std::ostream&, Copy_init const&);


#endif
