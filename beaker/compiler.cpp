// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "config.hpp"

#include "beaker/options.hpp"
#include "beaker/job.hpp"
#include "beaker/lexer.hpp"
#include "beaker/parser.hpp"
#include "beaker/decl.hpp"
#include "beaker/elaborator.hpp"
#include "beaker/generator.hpp"
#include "beaker/error.hpp"

#include <iostream>
#include <fstream>

// FIXME: It would be better if the generator hid all
// of these details from us.
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>


enum Target
{
  program_tgt,
  module_tgt
};


// Records the configuration parsed from the
// command line arguments.
struct Config
{
  bool keep     = false;
  bool assemble = false;
  bool compile  = false;
  Target target = program_tgt;
};


static void
usage(std::ostream& os, po::options_description& desc)
{
  os << "usage: beaker-compile [options] input-file...\n";
  os << desc << '\n';
}


static bool parse(Path const&, Config const&);
static bool parse(Path_seq const&, Path const&, Config const&);

static bool lower(Path const&, Path const&, Config const&);
static bool assemble(Path const&, Path const&, Config const&);
static bool executable(Path_seq const&, Path const&, Config const&);
static bool module(Path_seq const&, Path const&, Config const&);


// Global resources.
Location_map locs; // Source code locations
Symbol_table syms; // The symbol table
Module_decl  mod;  // The translation module


int
compiler_main(int argc, char* argv[])
{
  init_colors();
  init_symbols(syms);

  po::options_description common_opts("Common options");
  common_opts.add_options()
    ("help",      po::bool_switch(),        "Print this message and exit.")
    ("version",   po::bool_switch(),        "Print version information and exit.")
    ("input,i",   po::value<String_seq>(),  "Specify input files.")
    ("output,o",  po::value<String>(),      "Specify the output file.")
    ("keep,k",    po::bool_switch(),        "Keep temporary files.");

  // FIXME: These really define the compilation mode.
  // Here are some rules:
  //    -s implies -c
  //    -c implies no linking
  //    -t program|module
  po::options_description compile_opts("Compile options");
  compile_opts.add_options()
    ("assemble,s",  po::bool_switch(),  "Compile to native assembly.")
    ("compile,c",   po::bool_switch(),  "Compile but do not link.")
    ("target,t",    po::value<String>()->default_value("program"),
     "Specify whether a program or module should be produced.");

  po::positional_options_description positional_opts;
  positional_opts.add("input", -1);

  po::options_description all_opts;
  all_opts.add(common_opts).add(compile_opts);

  // Parse command line options.
  Config conf;
  po::variables_map vm;
  try {
    po::store(
      po::command_line_parser(argc, argv)
        .options(all_opts)
        .positional(positional_opts)
        .run(),
      vm);
    po::notify(vm);
  } catch (std::exception& err) {
    std::cerr << "error: " << err.what() << "\n\n";
    usage(std::cerr, all_opts);
    return -1;
  }

  // Check for obvious flags first.
  if (vm["help"].as<bool>()) {
    usage(std::cout, all_opts);
    return 0;
  }
  if (vm["version"].as<bool>()) {
    std::cout << PACKAGE_STRING << '\n';
    return 0;
  }

  // Check options.
  if (vm["compile"].as<bool>())
    conf.compile = true;

  if (vm["assemble"].as<bool>()) {
    conf.assemble = true;
    conf.compile = true;
  }

  String t = vm["target"].as<String>();
  if (t == "program") {
    conf.target = program_tgt;
  } else if (t == "module") {
    conf.target = module_tgt;
  } else  {
    std::cerr << "error: invalid build target\n\n";
    usage(std::cerr, all_opts);
    return -1;
  }

  // Validate the input files.
  if (!vm.count("input")) {
    std::cerr << "error: no input files\n\n";
    usage(std::cerr, all_opts);
    return -1;
  }
  Path_seq inputs;
  for (String const& s : vm["input"].as<String_seq>())
    inputs.push_back(s);

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

  // Parse all of the input files into the translation module.
  //
  // FIXME: We should collect a set of output files from
  // parsing since we could potentially pass .ll/.bc/.s/.o
  // files to the next phase of translation.
  //
  // FIXME: Clean up temporary files.
  Path ir = to_ir_file(output);
  if (!parse(inputs, ir, conf))
    return -1;

  Path as = to_asm_file(output);
  if (!lower(ir, as, conf))
    return -1;
  if (conf.assemble)
    return 0;

  Path obj = to_object_file(output);
  if (!assemble(as, obj, conf))
    return -1;
  if (conf.compile)
    return 0;

  // Generate the linked result.
  if (conf.target == program_tgt)
    return executable({obj}, output, conf);
  if (conf.target == module_tgt)
    return module({obj}, output, conf);

  return 0;
}


// Parse the input file into the module.
bool
parse(Path const& in, Config const& conf)
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
      return false;

    // Parse the token stream.
    Parser parse(syms, ts, locs);
    if (!parse.module(&mod))
      return false;

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


bool
parse(Path_seq const& in, Path const& out, Config const& conf)
{
  bool ok = true;
  for (Path const& p : in) {
    if (get_file_kind(p) == beaker_file)
      ok &= parse(p, conf);
    else {
      // FIXME: LLVM IR/BC or assembly could (should?) be
      // lowered and passed through to the link phase. That
      // would allow a module to contain native assembly,
      // and used internally via foreign declarations.
      std::cerr << "error: unknown input file type\n";
      return false;
    }
  }
  if (!ok)
    return false;

  // Elaborate the parse result.
  Elaborator elab(locs, syms);
  elab.elaborate(&mod);

  // Translate to LLVM.
  Generator gen;
  llvm::Module* ir = gen(&mod);

  // Write the output to an IR file, not the requested
  // output. That happens later.
  Path p = to_ir_file(out);

  // Write the result to the output file.
  std::error_code err;
  llvm::raw_fd_ostream ofs(p.string(), err, llvm::sys::fs::F_None);
  ofs << *ir;
  return true;
}


// Lower LLVM IR/BC to native assembly.
bool
lower(Path const& in, Path const& out, Config const& conf)
{
  Job job(llvm_compiler(), {
    format("-o {}", out.string()),
    in.string()
  });
  return job.run();
}


// Translate an assembly program into an object file.
//
// FIXME: We should be using the native assembler and
// not a C compiler for this program. Note that we
// add the "-c" option as a result of this decision.
bool
assemble(Path const& in, Path const& out, Config const& conf)
{
  Job job(native_assembler(), {
    "-c",
    format("-o {}", out.string()),
    in.string()
  });
  return job.run();
}


// Link a sequence of object files into an executable program.
// Note that this uses the C compiler, so we implicitly link
// against the C runtime.
//
// TODO: Don't link against the C runtime!
bool
executable(Path_seq const& in, Path const& out, Config const& conf)
{
  // Build the argument list.
  String_seq args;
  args.push_back(format("-o {}", out.string()));
  for (Path const& p : in)
    args.push_back(p.string());

  // Build and run the job.
  Job job(native_linker(), args);
  return job.run();
}


// Link a sequence of object files into a shared
// library.
bool
module(Path_seq const& in, Path const& out, Config const& conf)
{
  // Build the argument list.
  String_seq args;
  args.push_back("-shared");
  args.push_back(format("-o {}", out.string()));
  for (Path const& p : in)
    args.push_back(p.string());

  // Build and run the job.
  Job job(native_linker(), args);
  return job.run();
}
