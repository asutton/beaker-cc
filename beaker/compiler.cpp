// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "lexer.hpp"
#include "parser.hpp"
#include "elaborator.hpp"
#include "generator.hpp"
#include "decl.hpp"
#include "error.hpp"

#include <iostream>
#include <fstream>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>


using namespace std;


int
main(int argc, char* argv[])
{
  init_colors();

  // Prepare the symbol table.
  Symbol_table syms;
  init_symbols(syms);

  // Prepare the input buffer.
  //
  // FIXME: Handle command line arguments.
  File src = argv[1];
  Input_buffer in = src;

  try {
    // Create the token stream over. This will be populated
    // by the lexer.
    Token_stream ts;

    // Build and run the lexer.
    Lexer lex(syms, in);
    if (!lex.lex(ts))
      return -1;

    // Build and run the parser. The location map
    // is used to save source locations, which are
    // used to diagnose elaboration errors.
    Location_map locs;
    Parser parse(syms, ts, locs);
    Decl* m = parse.module();
    if (!parse)
      return -1;

    // Perform semantic analysis.
    //
    // TODO: Implement a parse-only phase.
    Elaborator elab(locs, syms);
    elab.elaborate(m);

    // If this is a module, generate a manifest
    // containing exported symbols.
    if (cast<Module_decl>(m)->module())
      std::ofstream mfs("a.bkm");

    // Translate to LLVM.
    //
    // TODO: Support translation to other models?
    Generator gen;
    llvm::Module* mod = gen(m);
    llvm::outs() << *mod;
  }

  // Diagnose uncaught translation errors and exit
  // gracefully. All other uncaught exceptions are
  // ICEs and we want those to fail noisily. Note
  // that re-throwing does not re-establish the
  // origin of the error for the purpose of debugging.
  catch (Translation_error& err) {
    diagnose(err);
    return -1;
  }

  // FIXME: Do something with the module.
}
