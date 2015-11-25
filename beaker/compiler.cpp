// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "options.hpp"
#include "job.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "elaborator.hpp"
#include "generator.hpp"
#include "error.hpp"

#include <iostream>
#include <fstream>


#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>


using namespace std;


void
usage(std::ostream& os, po::options_description& desc)
{
  os << "usage: beaker-compile [options] input-file...\n\n";
  os << desc << '\n';
}


int
main(int argc, char* argv[])
{
  Symbol_table syms;

  // Intitialize various subsystems.
  init_colors();
  init_symbols(syms);

  po::options_description common("common options");
  common.add_options()
    ("help",     "print help message")
    ("version",  "print version message")
    ("input,i",   po::value<String_seq>(), "specify input files")
    ("output,o",  po::value<String>(),     "specify the output file");

  po::options_description compiler("compile options");
  compiler.add_options()
    ("compile,c", "compile to object files");

  po::options_description all;
  all.add(common)
     .add(compiler);

  po::positional_options_description pos;
  pos.add("input", -1);

  po::variables_map vm;
  po::store(
    po::command_line_parser(argc, argv)
      .options(all)
      .positional(pos)
      .run(),
    vm);
  po::notify(vm);

  // Canonicalize the name of the program.
  Path argv0 = fs::canonical(argv[0]);

  std::cout << argv0 << '\n';

#if 0
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

  // Validate the input files.
  if (!vm.count("input")) {
    std::cerr << "error: no input files given\n";
    usage(std::cerr, all);
    return -1;
  }
  String_seq inputs = vm["input"].as<String_seq>();

  // Look for an output file. If not given, assume that
  // the end result is going to be a native binary. Note
  // that this is the end goal for inputs of any variety.
  String output;
  if (!vm.count("output")) {
    // TODO: This should depend on the compilation mode.
    output = "a.out";
  } else {
    output = vm["output"].as<String>();
  }

  // FIXME: Handle the case where there is a single
  // beaker input and non-linked output file, then
  // just run that actions.
  if (inputs.size() == 1) {
    std::cout << "do front-end action" << '\n';
    return 0;
  }

  // TODO: If there are multiple inputs and a non-linked 
  // output file, that's an error.
  if (inputs.size() > 1) {
    File_kind k = get_file_kind(output);
    if (!is_linked_file(k)) {
      std::cerr << "error: invlaid output specified for multiple inputs\n";
      return -1;
    }
  }


  // Produce a series of jobs that lowers all inputs
  // to object files.
  Job_seq jobs;
  String_seq outputs;
  for (String const& str : inputs) {
    Path in = str;
    Path out;

    File_kind k = get_file_kind(in);
    if (k == beaker_file) {
      out = to_ir_file(in);
      String_seq args {
        format("-o {}", out.string()),
        in.string()
      };
      jobs.emplace_back(argv0.string(), args);
      in.swap(out);
      k = ir_file;
    }

    if (k == ir_file) {
      out = to_ir_file(in);
      String_seq args {
        format("-o {}", out.string()),
        in.string()
      };
      jobs.emplace_back(llvm_compiler(), args);
      in.swap(out);
      k = asm_file;
    }

    if (k == asm_file) {
      out = to_object_file(in);
      // FIXME: The "-c" is necessary only when we are using
      // the C compiler as the assembler. It would be better
      // if we abstracted over this detail.
      String_seq args {
        "-c",
        format("-o {}", out.string()),
        in.string()
      };
      jobs.emplace_back(native_assembler(), args);
      in.swap(out);
      k = object_file;
    }
    
    // Save the final output file.
    outputs.emplace_back(in.string());
  }

  // If needed, perform the final linker stage.
  //
  // FIXME: Don't compiler if -c is set.
  File_kind k = get_file_kind(output);
  if (k == program_file) {
    String_seq args = outputs;
    args.emplace_back(format("-o {}", output));
    jobs.emplace_back(native_linker(), args);
  }

  // Run all of the jobs.
  for (Job& j : jobs)
    j.run();

#endif

  #if 0

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

  #endif
}
