// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "options.hpp"
#include "job.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "decl.hpp"
#include "elaborator.hpp"
#include "generator.hpp"
#include "error.hpp"

#include <iostream>
#include <fstream>

// FIXME: It would be better if the generator hid all
// of these details from us.
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>


enum Target
{
  module_tgt,
  program_tgt,
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
  os << "usage: beaker-compile [options] input-files...\n\n";
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

  po::options_description common("common options");
  common.add_options()
    ("help",     "print help message")
    ("version",  "print version message")
    ("input,i",   po::value<String_seq>(), "specify build inputs")
    ("output,o",  po::value<String>(),     "specify the build output file")
    ("keep,k",    "keep temporary files");

  // FIXME: These really define the compilation mode.
  // Here are some rules:
  //
  //    -s implies -c
  //    -c implies no linking
  //
  //    -b [archive|library|program]
  //
  po::options_description compiler("compile options");
  compiler.add_options()
    ("assemble,s", "compile to native assembly")
    ("compile,c",  "compile but do not link")
    ("target,t",   po::value<String>()->default_value("program"),
                   "produce an archive, module, or program");

  po::positional_options_description pos;
  pos.add("input", -1);

  po::options_description all;
  all.add(common)
     .add(compiler);

  // Parse command line options.
  Config conf;
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
    // TODO: Generate the version number from the build.
    std::cout << "beaker v0.0" << '\n';
    return 0;
  }

  // Check options.
  if (vm.count("compile"))
    conf.compile = true;

  if (vm.count("assemble")) {
    conf.assemble = true;
    conf.compile = true;
  }

  if (vm.count("target")) {
    String t = vm["target"].as<String>();
    if (t == "program") {
      conf.target = program_tgt;
    } else if (t == "module") {
      conf.target = module_tgt;
    } else  {
      std::cerr << "error: invalid build target\n";
      usage(std::cerr, all);
      return -1;
    }
  }

  // Validate the input files.
  if (!vm.count("input")) {
    std::cerr << "error: no input files given\n";
    usage(std::cerr, all);
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
      std::cerr << "error: unknown input file\n";
      return -1;
    }
  }
  if (!ok)
    return -1;

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
