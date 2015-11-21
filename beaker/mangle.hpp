// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_MANGLE_HPP
#define BEAKER_MANGLE_HPP

// This module supports the mangling of identifers
// suitable for linking.

#include "prelude.hpp"


String mangle(Type const*);
String mangle(Expr const*);
String mangle(Decl const*);

void mangle(std::ostream&, Type const*);
void mangle(std::ostream&, Id_type const*);
void mangle(std::ostream&, Boolean_type const*);
void mangle(std::ostream&, Character_type const*);
void mangle(std::ostream&, Integer_type const*);
void mangle(std::ostream&, Function_type const*);
void mangle(std::ostream&, Array_type const*);
void mangle(std::ostream&, Block_type const*);
void mangle(std::ostream&, Reference_type const*);
void mangle(std::ostream&, Record_type const*);


void mangle(std::ostream&, Decl const*);
void mangle(std::ostream&, Variable_decl const*);
void mangle(std::ostream&, Function_decl const*);
void mangle(std::ostream&, Parameter_decl const*);
void mangle(std::ostream&, Record_decl const*);
void mangle(std::ostream&, Field_decl const*);
void mangle(std::ostream&, Method_decl const*);
void mangle(std::ostream&, Module_decl const*);

#endif
