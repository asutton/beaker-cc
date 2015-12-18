// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "config.hpp"
#include "beaker/options.hpp"
#include "beaker/job.hpp"
#include "beaker/lexer.hpp"
#include "beaker/parser.hpp"
#include "beaker/elaborator.hpp"
#include "beaker/generator.hpp"
#include "beaker/error.hpp"

#include <iostream>
#include <fstream>

#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>


// Global resources.
Symbol_table syms;


static void
usage(std::ostream& os, po::options_description& desc)
{
  os << "usage: beaker-translate [options] <input>\n";
  os << desc << '\n';
}


// FIXME: These should take arguments that have been
// passed through the command line.
static bool compile(Path const&, Path const&);
static bool lower(Path const&, Path const&);


int
translator_main(int argc, char* argv[])
{
  // Intitialize various subsystems.
  //
  // TODO: Always initialize parsing components?
  init_colors();
  init_symbols(syms);

  // TODO: Support extraction to other intermediate formats?
  // Maybe C, for example? Gimple?
  //
  // Note that an introspection tool would have a considerably
  // different interface.
  po::options_description common("options");
  common.add_options()
    ("help",     "print help message")
    ("version",  "print version message")
    ("input,i",   po::value<String>(), "specify the input file")
    ("output,o",  po::value<String>(), "specify the output file")
    ("keep,k",    "keep temporary files");

  po::positional_options_description pos;
  pos.add("input", 1);

  po::options_description all;
  all.add(common);

  // Parse command line options.
  po::variables_map vm;
  try {
    po::store(
      po::command_line_parser(argc, argv)
        .options(all)
        .positional(pos)
        .run(),
      vm);
    po::notify(vm);
  } catch(std::exception& err) {
    std::cerr << "error: " << err.what() << "\n\n";
    usage(std::cerr, all);
    return -1;
  }

  // Check for obvious flags first.
  if (vm.count("help")) {
    usage(std::cout, all);
    return 0;
  }

  if (vm.count ("version")) {
    // TODO: Generate the version number from the
    // build.
    std::cout << "beaker v0.0" << '\n';
    return 0;
  }

  // Validate the input.
  if (!vm.count("input")) {
    std::cerr << "error: no input file given\n";
    usage(std::cerr, all);
    return -1;
  }
  Path input = vm["input"].as<String>();


  // Look for an output file. If not given, the default
  // will be to produce .asm. Note that the default output
  // file is generated in the current directory.
  Path output;
  if (!vm.count("output"))
    output = to_asm_file(input.filename());
  else
    output = vm["output"].as<String>();

  // Compile the input into LLVM.
  Path temp;
  if (get_file_kind(input) == beaker_file) {
    temp = to_ir_file(input.filename());
    if (!compile(input, temp))
      return -1;
    input = temp;
  }

  // Lower llvm input to assembly.
  if (!lower(input, output))
    return -1;

  // Remove the temporary file.
  if (!temp.empty() && !vm.count("keep"))
    fs::remove(temp);

  return 0;
}


bool
compile(Path const& in, Path const& out)
{
  try {
    // Read the input source.
    File src = in.c_str();
    Input_buffer buf = src;

    // Lex the input source.
    Token_stream ts;
    Location_map locs;
    Lexer lex(syms, buf);
    if (!lex.lex(ts))
      return -1;

    // Parse the token stream.
    Parser parse(syms, ts, locs);
    Decl* m = parse.module();
    if (!parse)
      return -1;

    // Perform semantic analysis.
    Elaborator elab(locs, syms);
    elab.elaborate(m);

    // Translate to LLVM.
    Generator gen;
    llvm::Module* mod = gen(m);

    std::error_code err;
    llvm::raw_fd_ostream ofs(out.string(), err, llvm::sys::fs::F_None);
    ofs << *mod;
    return true;
  }

  // Diagnose uncaught translation errors and exit
  // gracefully. All other uncaught exceptions are
  // ICEs and we want those to fail noisily. Note
  // that re-throwing does not re-establish the
  // origin of the error for the purpose of debugging.
  catch (Translation_error& err) {
    diagnose(err);
    return false;
  }
}
