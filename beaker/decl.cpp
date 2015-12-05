// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/decl.hpp"
#include "beaker/type.hpp"
#include <iostream>


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


std::vector<int>
Field_decl::index() const
{
	auto current = context();
	std::vector<int> ret;
	Decl_seq const& f = current->fields();
    int i = current->fields().size();
	for (int i = 0; i < f.size(); ++i)
		if (f[i] == this) {
		  ret.push_back(i);
		  return ret;
		}

	while(current->base_decl != nullptr) {
		current = current->base_decl;
		ret.push_back(0);
		Decl_seq const& f = current->fields();
		for (int i = 0; i < f.size(); ++i)
			if (f[i] == this) {
		  		ret.push_back(i);
		  		return ret;
			}
	}
	//if the last element is -1, it does not exist
	ret.push_back(-1);
	return ret;
}


// Returns true when the declaration is declared as
// a reference to an object.
bool
is_reference(Decl const* d)
{
  return is<Reference_type>(d->type());
}
