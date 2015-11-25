// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "file.hpp"


File::File(char const* p)
  : path_(boost::filesystem::canonical(p))
{ }


// Returns the kind of file associated with the input
// text. Note that
File_kind
get_file_kind(Path const& p)
{
  Path ext = p.extension();
  if (ext == ".bkr")
    return beaker_file;
  if (ext == ".ll")
    return ir_file;
  if (ext == ".bc")
    return bitcode_file;
  if (ext == ".s")
    return asm_file;
  if (ext == object_extension())
    return object_file;
  if (ext == library_extension())
    return library_file;
  if (ext == archive_extension())
    return archive_file;
  if (ext == executable_extension())
    return program_file;
  if (ext == ".out")
    return program_file;
  else
    return unspecified_file; 
}
