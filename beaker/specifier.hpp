// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SPECIFIER_HPP
#define BEAKER_SPECIFIER_HPP


// Declaration specifiers. These determine the
// semantics of declarations.
//
// TODO: Other specifiers? Virtual?
enum Specifier
{
  // No specifiers present.
  no_spec      = 0,

  // The declaration is exported from the module.
  // By default declarations are not exported.
  export_spec = 1 << 9,

  // The declaration has foreign language linkage.
  // This affects  how the symbol is generated during
  // translation. By default, a declaration with the
  // foreign specifier has C language linkage.
  // Otherwise, it has Beaker language linkage.
  //
  // Only functions and variables can have foreign
  // language linkage.
  //
  // TODO: Support foreign language linkage for other
  // other languages?
  foreign_spec = 1 << 10,
};


inline Specifier&
operator|=(Specifier& a, Specifier b)
{
  unsigned x = a | b;
  a = Specifier(x);
  return a;
}


#endif
