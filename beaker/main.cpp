// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "lexer.hpp"
#include "parser.hpp"
#include "elaborator.hpp"
#include "evaluator.hpp"
#include "generator.hpp"
#include "error.hpp"

#include "llvm/llvm.hpp"

#include <iostream>
#include <fstream>


using namespace std;


void init_symbols(Symbol_table&);


int 
main(int argc, char* argv[])
{
  // FIXME: This is gross. Do one or the other, but
  // never both.
  bool evaluate = false;

  // Prepare the symbol table.
  Symbol_table syms;
  init_symbols(syms);

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

    // Build and run the parser.
    Parser parse(syms, ts);
    Decl* m = parse.module();
    if (!parse)
      return -1;

    // TODO: Implement a parse-only phase.
    Elaborator elab;
    elab.elaborate(m);
    
    // Find an entry point for evaluation.
    //
    // TODO: Actually pass command line arguments.
    if (evaluate) {
      if (elab.main) {
        Evaluator ev;
        Value v = ev.exec(elab.main);
        std::cout << v.r.z << '\n';
      } else {
        std::cout << "no main\n";
      }
    }

    // Otherwise, translate to LLVM.
    else {
      Generator gen;
      ll::Decl* r = gen.gen(m);

      using lingo::print;
      print(r);
    }


  } catch (Translation_error& err) {
    diagnose(err);
  } catch (std::runtime_error& err) {
    std::cout << "internal compiler error:" << err.what() << '\n';
  }

  // FIXME: Do something with the module.
}


void
init_symbols(Symbol_table& syms)
{
  // Create the symbol table and install all of the 
  // default tokens.
  syms.put<Symbol>("{", lbrace_tok);
  syms.put<Symbol>("}", rbrace_tok);
  syms.put<Symbol>("(", lparen_tok);
  syms.put<Symbol>(")", rparen_tok);
  syms.put<Symbol>(",", comma_tok);
  syms.put<Symbol>(":", colon_tok);
  syms.put<Symbol>(";", semicolon_tok);
  syms.put<Symbol>("=", equal_tok);
  syms.put<Symbol>("+", plus_tok);
  syms.put<Symbol>("-", minus_tok);
  syms.put<Symbol>("*", star_tok);
  syms.put<Symbol>("/", slash_tok);
  syms.put<Symbol>("%", percent_tok);
  syms.put<Symbol>("==", eq_tok);
  syms.put<Symbol>("!=", ne_tok);
  syms.put<Symbol>("<", lt_tok);
  syms.put<Symbol>(">", gt_tok);
  syms.put<Symbol>("<=", le_tok);
  syms.put<Symbol>(">=", ge_tok);
  syms.put<Symbol>("&&", and_tok);
  syms.put<Symbol>("||", or_tok);
  syms.put<Symbol>("!", not_tok);
  syms.put<Symbol>("->", arrow_tok);

  // Keywords
  syms.put<Symbol>("var", var_kw);
  syms.put<Symbol>("def", def_kw);
  syms.put<Symbol>("bool", bool_kw);
  syms.put<Symbol>("int", int_kw);
  syms.put<Symbol>("return", return_kw);
  
  // Reserved names.
  syms.put<Boolean_sym>("true", boolean_tok, true);
  syms.put<Boolean_sym>("false", boolean_tok, false);
}