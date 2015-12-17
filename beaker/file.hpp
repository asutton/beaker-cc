// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_FILE_HPP
#define BEAKER_FILE_HPP

#include <beaker/prelude.hpp>

#include <boost/filesystem.hpp>


namespace fs = boost::filesystem;


// A path represents the location of a file in the
// file system.
using Path = fs::path;


// A sequence of paths.
using Path_seq = std::vector<Path>;


// An input file.
class File
{
public:
  File(char const*);

  Path const&   path() const     { return path_; }
  String const& pathname() const { return path_.string(); }

private:
  Path path_;
};


// -------------------------------------------------------------------------- //
// Kinds of files

// The different kinds of files that can be operated
// on by tools in the toolchain.
enum File_kind
{
  unspecified_file, // Not one of the files below.

  // Input languages
  beaker_file,     // Beaker source text
  
  // Intermediate languages
  ir_file,         // LLVM source text
  bitcode_file,    // LLVM bitcode
  asm_file,        // Native assembly source text

  // Binary representations
  object_file,     // Native objectd file
  library_file,    // Dynamic libraries (.so/.dylib/.dll)
  archive_file,    // Static libraries (.a/.lib)
  program_file     // Executable programs (<noext>/.exe)
};


File_kind get_file_kind(Path const&);


// Returns true if the file kind is the product
// of linking.
inline bool
is_linked_file(File_kind k)
{
  switch (k) {
    case library_file:
    case archive_file:
    case program_file:
      return true;
    default:
      return false;
  }
}


// Return a new path by replacing a .bkr extension
// with a .ll extension.
inline Path
to_ir_file(Path p)
{
  return p.replace_extension(".ll");
}


// Return a new path by replacing a .ll extension
// with a .s extension.
inline Path
to_asm_file(Path p)
{
  return p.replace_extension(".s");
}


// Return a new path by replacing a .s extension
// with a .o extension.
inline Path
to_object_file(Path p)
{
  return p.replace_extension(object_extension());
}


#endif
