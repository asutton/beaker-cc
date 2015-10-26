// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "string.hpp"

#include <iostream>
#include <iterator>


using Iter = std::istreambuf_iterator<char>;


// -------------------------------------------------------------------------- //
// String buffers

// Read the contents of the given input stream
// into the string buffer.
Stringbuf::Stringbuf(std::istream& is)
  : buf_(Iter(is), Iter())
{ }


void
Stringbuf::assign(std::istream& is)
{ 
  buf_.assign(Iter(is), Iter());
}

