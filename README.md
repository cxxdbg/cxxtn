# cxxtn

A C++ library for parsing demangled C++ type and function names (the
human-readable strings produced by GCC's/libstdc++'s and Clang's/libc++'s
demanglers, e.g. `abi::__cxa_demangle`) into a structured AST, and writing
that AST back out as a string in either GCC or Clang style.

The parsed representation distinguishes qualified types, pointers,
references, arrays, function types, templates, scopes, and builtins, so
callers can inspect or transform a type name (for example, to compare a
type across GCC and Clang standard library implementations, or to
reconstruct a canonical form) without re-implementing a demangled-name
grammar themselves.

`cxxtn` was originally developed as part of the [cxxdbg](https://github.com/cxxdbg)
debugger's STL value formatters, which need to identify container/value
types from their demangled names across different standard library
implementations.
