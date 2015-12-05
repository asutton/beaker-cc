// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include <iostream>
#include "decl.hpp"
#include "type.hpp"


Function_type const*
Function_decl::type() const
{
  return cast<Function_type>(Decl::type());
}


Type const*
Function_decl::return_type() const
{
  return type()->return_type();
}


int
Field_decl::index() const
{
	auto current = context();
	int ret = 0;
	Decl_seq const& f = current->fields();
	for (int i = 0; i < f.size(); ++i)
		if (f[i] == this) {
		  ret = i;
			// std::cout<< ret <<"infirst\n";
		  // return ret;
		}

	while(current->base_decl != nullptr) {

		ret++;
		current = current->base_decl;
	}
	std::cout<< ret <<"out\n";
	return ret;
}


// Returns true when the declaration is declared as
// a reference to an object.
bool
is_reference(Decl const* d)
{
  return is<Reference_type>(d->type());
}
