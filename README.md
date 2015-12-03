# beaker

The Beaker Programming Language.


## Dependencies

Beaker has several external dependencies.

- boost 1.55 or higher.
- asutton/lingo is a library that provides a number of utilities that can
  be used within various compilers. This
- The csll/llvm library provides a lightweight text-based LLVM
  assembly generator.

Note that Beaker does not depend on LLVM directly because the LLVM flags
disable exceptions, which beaker relies upon heavily.


## Building Beaker

After cloning the repository, go into the directory and run:

~~~
git submodule update --init --recursive
~~~

This will pull the required versions of asutton/lingo and
csll/llvm into your working directory.

Note that if you have a fork of beaker, you will need to
update lingo submodule.

~~~
git submodule update --remote --merge
~~~

The build is configured by CMake.

~~~
mkdir build
cd build
cmake ..
make
~~~



## Running the compiler

The compiler is run by the command `beaker`. It takes a single
input file as an argument:

~~~
./beaker input.bkr
~~~

If `input`.bkr has a function named `main`, that function is
evaluated (`main` must not take arguments). Otherwise, the
compiler simply prints `no main`.


## Testing

There is a test directory within hbe


## Notes

The beaker implementation does not (currently) directly depend on
lingo. Eventually, beaker will depend on the following features
in lingo:
  - string formatting
  - node concepts
  - debugging macros
  - sexpr debugging

The long-term plan is to migrate a number of features *out* of
beaker and into lingo, replacing older, less capable implementaitons.
These are likely to be:
  - the symbol table
  - input buffer
  - tokens
  - lexing and parsing
  - generic environments
  - ???

## Errata

When lowering LLVM to native assembly on new versions of Mac OS X
you may need to override the system triple to avoid linker warnings.
Invoke `llc` like this:

```
llc -mtriple=x86_64-apple-macosx <input>
```
