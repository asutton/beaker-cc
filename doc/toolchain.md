
# Beaker Toolchain

What are the various tools:

Common compilation pass:
1. Translate beaker to LLVM ASM/BC.
Phase 1 is the beaker front end compiler. We may also generate a
supplemental manifest that lists exported declarations of a module.

2. Translate LLVM ASM/BC to object code
Phase 2 uses the LLVM static compiler (LLC).

3. Combine objects in archives
Archive objects?

4. Link objects to shared objects
Uses the native compiler.

5. Link objects to program executable
Uses the native compiler.
