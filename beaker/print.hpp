// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PRINT_HPP
#define BEAKER_PRINT_HPP

#include "prelude.hpp"

#include <iosfwd>

std::ostream& operator<<(std::ostream&, Type const*);
std::ostream& operator<<(std::ostream&, Boolean_type const*);
std::ostream& operator<<(std::ostream&, Integer_type const*);
std::ostream& operator<<(std::ostream&, Function_type const*);
std::ostream& operator<<(std::ostream&, Record_type const*);

// network specific types
std::ostream& operator<<(std::ostream&, Table_type const*);
std::ostream& operator<<(std::ostream&, Flow_type const*);
std::ostream& operator<<(std::ostream&, Port_type const*);

#endif
