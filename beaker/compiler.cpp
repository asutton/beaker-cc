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



enum Build_kind
{
  library_build,
  program_build,
};


// Records the configuration parsed from the
// command line arguments.
struct Config
{
  bool keep     = false;
  bool assemble = false;
  bool compile  = false;
  int  build    = program_build;
};


static void
usage(std::ostream& os, po::options_description& desc)
{
  os << "usage: beaker-compile [options] input-file...\n\n";
  os << desc << '\n';
}


static bool translate(Path const&, Path const&, Config const&);
static bool assemble(Path const&, Path const&, Config const&);
static bool executable(Path_seq const&, Path const&, Config const&);
static bool library(Path_seq const&, Path const&, Config const&);



int
compiler_main(int argc, char* argv[])
{
  init_colors();

  po::options_description common("common options");
  common.add_options()
    ("help",     "print help message")
    ("version",  "print version message")
    ("input,i",   po::value<String_seq>(), "specify input files")
    ("output,o",  po::value<String>(),     "specify the output file")
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
    ("compile,c",  "compile to object files")
    ("build,b",     po::value<String>()->default_value("program"),
                   "produce a library or program");

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
    // TODO: Generate the version number from the 
    // build.
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

  if (vm.count("build")) {
    String b = vm["build"].as<String>();
    if (b == "program") {
      conf.build = program_build;
    } else if (b == "library") {
      conf.build = library_build;
    } else  {
      std::cerr << "error: invalid build specification\n";
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

  // TODO: If there are multiple inputs and a non-linked 
  // output file, that's an error.
  if (inputs.size() > 1) {
    File_kind k = get_file_kind(output);
    if (!is_linked_file(k)) {
      std::cerr << "error: invalid output specified for multiple inputs\n";
      return -1;
    }
  }

  // Produce a series of jobs that lowers all inputs
  // to object files.
  //
  // TODO: It might be worthwhile to produce a series or
  // graph of jobs rather than doing each as needed. This
  // would allow for parallel builds.
  Path_seq outputs;
  Path_seq temps;
  for (String const& str : inputs) {
    Path in = str;

    // Translate to assembly.
    File_kind k = get_file_kind(in);
    if (k == beaker_file || k == ir_file || k == bitcode_file) {
      Path temp = to_asm_file(in.filename());
      if (!translate(in, temp, conf))
        return -1;
      if (!conf.assemble)
        temps.push_back(temp);
      in = temp;
      k = asm_file;
    }

    // If -s is turned on, then don't generate object files.
    if (conf.assemble)
      continue;

    // Produce object code.
    if (k == asm_file) {
      Path temp = to_object_file(in.filename());
      if (!assemble(in, temp, conf))
        return -1;
      if (!conf.compile)
        temps.push_back(temp);
      in = temps.back();
    }

    // Save outputs.    
    outputs.emplace_back(in);
  }

  // If only compiling, stop here.
  if (conf.compile)
    return 0;

  // Produce an executable or a library.
  bool ok = false;
  if (conf.build == program_build)
    ok = executable(outputs, output, conf);
  else if (conf.build == library_build)
    ok = library(outputs, output, conf);

  // FIXME: WE
  if (!conf.keep) {
    for (Path const& p : temps)
      fs::remove(p);
  }

  return ok ? 0 : -1;
}


// Lower an input source to a native assembly file
// using translate program.
bool 
translate(Path const& in, Path const& out, Config const& conf)
{
  extern Path translator;
  
  // Build the argument sequence.
  String_seq args;
  if (conf.keep)
    args.push_back("-k");
  args.push_back(format("-o {}", out.string()));
  args.push_back(in.string());

  // Build and run the job.
  Job job(translator.string(), args);
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


// Link a sequence of object files into an executable
// program. Note that this uses the C compiler, so
// we implicitly link against the C runtime.
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
library(Path_seq const& in, Path const& out, Config const& conf)
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
