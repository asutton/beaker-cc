// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/file.hpp"


File::File(char const* p)
  : path_(boost::filesystem::canonical(p))
{ }
