# Beaker

The Beaker Programming Language.

## Dependencies

Beaker has several external dependencies.

- [Boost](http://www.boost.org) version 1.55 or later.
- [Lingo](https://github.com/asutton/lingo) is a library that provides a
  number of utilities that can be used within various compilers.
- The [csll/llvm](https://github.com/csll/llvm) library provides a
  lightweight text-based LLVM assembly generator.

Note that Beaker does not depend on LLVM directly because the LLVM flags
disable exceptions, which Beaker relies upon heavily.

## Building Beaker

After cloning the repository, go into the directory and run:

```shell
git submodule update --init --recursive
```

This will pull the required versions of Lingo into your working directory.

Note that if you have a fork of Beaker, you will need to update the 'lingo'
submodule.

```shell
git submodule update --remote --merge
```

The build is configured by [CMake](https://cmake.org).

```shell
mkdir build
cd build
cmake ..
make
```

## Running the compiler

The compiler is run by the command `beaker`. It takes the name of a single
input file as an argument:

```shell
./beaker input.bkr
```

If `input.bkr` has a function named `main()`, that function is evaluated
(`main()` must not take arguments). Otherwise, the compiler simply prints
"no main".

## Notes

The Beaker implementation does not (currently) directly depend on Lingo.
Eventually, Beaker will depend on the following features in Lingo:

- String formatting
- Node concepts
- Debugging macros
- S-expression debugging

The long-term plan is to migrate a number of features *out* of Beaker and
into Lingo, replacing older, less capable implementations. These are likely
to be:

- The symbol table
- Input buffer
- Tokens
- Lexing and parsing
- Generic environments
- ???

## Errata

When lowering LLVM to native assembly on new versions of Mac OS X you may
need to override the system triple to avoid linker warnings. Invoke `llc`
like this:

```shell
llc -mtriple=x86_64-apple-macosx <input>
```
