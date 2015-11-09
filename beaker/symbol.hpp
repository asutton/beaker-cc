// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SYMBOL_HPP
#define BEAKER_SYMBOL_HPP

#include "string.hpp"

#include "lingo/node.hpp"

#include <unordered_map>
#include <typeinfo>


// -------------------------------------------------------------------------- //
//                            Symbols


// The base class of all symbols of a language. By
// itself, this class is capable of representing
// symbols that have no other attributes such as
// punctuators and operators.
class Symbol
{
  friend struct Symbol_table;

public:
  Symbol(int);

  virtual ~Symbol() { }

  String const& spelling() const;
  int           token() const;

private:
  String const* str_; // The textual representation
  int           tok_; // The associated token kind
};


inline
Symbol::Symbol(int k)
  : str_(nullptr), tok_(k)
{ }


// Returns the spelling of the symbol.
inline String const&
Symbol::spelling() const
{
  return *str_;
}


// Returns the kind of token classfication of
// the symbol.
inline int
Symbol::token() const
{
  return tok_;
}


// Represents the integer symbols true and false.
struct Boolean_sym : Symbol
{
  Boolean_sym(int k, bool b)
    : Symbol(k), value_(b)
  { }

  bool value() const { return value_; }

  bool value_;
};


// Represents all integer symbols.
//
// TODO: Develop and use a good arbitrary precision
// integer for this representation.
//
// TOOD: Track the integer base? Technically, that
// can be inferred by the spelling, but it might be
// useful to keep cached.
struct Integer_sym : Symbol
{
  Integer_sym(int k, int n)
    : Symbol(k), value_(n)
  { }

  int value() const { return value_; }

  int value_;
};


// Represents all identifiers.
//
// TODO: Track the innermost binding of the identifier?
struct Identifier_sym : Symbol
{
  Identifier_sym(int k)
    : Symbol(k)
  { }
};


// Streaming
std::ostream& operator<<(std::ostream&, Symbol const&);


// -------------------------------------------------------------------------- //
//                           Symbol table


// The symbol table maintains a mapping of
// unique string values to their corresponding
// symbols.
struct Symbol_table : std::unordered_map<std::string, Symbol*>
{
  ~Symbol_table();

  template<typename T, typename... Args>
  Symbol* put(String const&, Args&&...);

  template<typename T, typename... Args>
  Symbol* put(char const*, char const*, Args&&...);

  Symbol const* get(String const&) const;
  Symbol const* get(char const*) const;
};


// Delete allocated resources.
inline
Symbol_table::~Symbol_table()
{
  for (auto const& x : *this)
    delete x.second;
}


// Insert a new symbol into the table. The spelling
// of the symbol is given by the string s and the
// attributes are given in args.
//
// Note that the type of the symbol must be given
// explicitly, and it must derive from the Symbol
// class.
//
// If the symbol already exists, no insertion is
// performed. If new symbol is of a different kind
// (e.g., redefining an integer as an identifier),
// a runtime error is thrown.
//
// TODO: It might also be helpful to verify the
// attributes of re-inserted symbols. That's a bit
// harder.
template<typename T, typename... Args>
Symbol*
Symbol_table::put(String const& s, Args&&... args)
{
  auto x = emplace(s, nullptr);
  auto iter = x.first;
  Symbol*& sym = iter->second;
  if (x.second) {
    // Insertion succeeded, so create a new symbol
    // and bind its string representation.
    sym = new T(std::forward<Args>(args)...);
    sym->str_ = &iter->first;
  } else {
    // Insertion did not succeed. Check that we have
    // not redefined the symbol kind.
    if (typeid(T) != typeid(*sym))
      throw std::runtime_error("redefinition of symbol");
  }
  return iter->second;

}


// Insert a symbol with the spelling [first, last) and
// the properties in args...
template<typename T, typename... Args>
inline Symbol*
Symbol_table::put(char const* first, char const* last, Args&&... args)
{
  return this->template put<T>(String(first, last), std::forward<Args>(args)...);
}


// Returns the symbol with the given spelling or
// nullptr if no such symbol exists.
inline Symbol const*
Symbol_table::get(String const& s) const
{
  auto iter = find(s);
  if (iter != end())
    return iter->second;
  else
    return nullptr;
}


// Returns the symbol with the given spelling or
// nullptr if no such symbol exists.
inline Symbol const*
Symbol_table::get(char const* s) const
{
  auto iter = find(s);
  if (iter != end())
    return iter->second;
  else
    return nullptr;
}


#endif
