// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "beaker/lexer.hpp"
#include "beaker/parser.hpp"
#include "beaker/decl.hpp"
#include "beaker/elaborator.hpp"
#include "beaker/evaluator.hpp"
#include "beaker/generator.hpp"
#include "beaker/error.hpp"

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

  Module_decl mod;

  // Prepare the input buffer.
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
    if (!parse.module(&mod))
      return -1;

    // Perform semantic analysis.
    //
    // TODO: Implement a parse-only phase.
    Elaborator elab(locs, syms);
    elab.elaborate(&mod);

    // Find an entry point for evaluation.
    //
    // TODO: The resolution of main is a little artificial.
    // Also, we need to ensure that static initializers
    // are evaluated prior to entering main.
    //
    // TODO: Actually pass command line arguments to main.
    if (elab.main) {
      Evaluator ev;
      Value v = ev.exec(elab.main);
      std::cout << "result: " << v << '\n';
    } else {
      std::cout << "no main\n";
    }
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
