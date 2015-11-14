// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_ERROR_HPP
#define BEAKER_ERROR_HPP

#include "prelude.hpp"
#include "location.hpp"

#include <stdexcept>


// A translation error is a general class of runtime 
// errors that occur at a particular point in the
// input source.
class Translation_error : public std::runtime_error
{
public:
  Translation_error(Location loc, char const* what)
    : std::runtime_error(what), loc_(loc)
  { }

  Translation_error(Location loc, String const& what)
    : std::runtime_error(what), loc_(loc)
  { }

  Location location() const { return loc_; }

private:
  Location loc_;
};


// Represents a lexical error.
struct Lexical_error : Translation_error
{
  using Translation_error::Translation_error;
};



// Represents a syntax error.
struct Syntax_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents a type checking error.
struct Lookup_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents a type checking error.
struct Type_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents an error that occurs during
// compile-time evaluation or interpretation.
struct Evaluation_error : Translation_error
{
  using Translation_error::Translation_error;
};


void diagnose(Translation_error&);


#endif
