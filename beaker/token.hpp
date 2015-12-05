// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_TOKEN_HPP
#define BEAKER_TOKEN_HPP

#include <beaker/prelude.hpp>
#include <beaker/symbol.hpp>
#include <beaker/location.hpp>

#include <list>


// -------------------------------------------------------------------------- //
//                            Token kinds

// The different kinds of tokens. Note that we need an
// error token to indicate the invalid tokens returned
// during lexing.
enum Token_kind
{
  error_tok = -1,

  // Punctuators and operators
  lbrace_tok,
  rbrace_tok,
  lparen_tok,
  rparen_tok,
  lbrack_tok,
  rbrack_tok,
  squote_tok,
  dquote_tok,
  comma_tok,
  colon_tok,
  semicolon_tok,
  dot_tok,
  equal_tok,
  plus_tok,
  minus_tok,
  star_tok,
  slash_tok,
  percent_tok,
  eq_tok,
  ne_tok,
  lt_tok,
  gt_tok,
  le_tok,
  ge_tok,
  and_tok,
  or_tok,
  not_tok,
  amp_tok,
  arrow_tok,

  // Keywords
  bool_kw,
  break_kw,
  char_kw,
  continue_kw,
  def_kw,
  else_kw,
  foreign_kw,
  if_kw,
  int_kw,
  return_kw,
  struct_kw,
  this_kw,
  var_kw,
  while_kw,
  signed_kw,
  unsigned_kw,
  short_kw,
  long_kw,
  int16_kw,
  int32_kw,
  int64_kw,
  int128_kw,
  float_kw,
  double_kw,

  // Multi-valued tokens
  boolean_tok,      // true | false
  integer_tok,      // digit+
  character_tok,    // narrow characters
  string_tok,       // narrow string literals
  identifier_tok,   // letter (letter | digit)*
};


char const*
spelling(Token_kind k);



// -------------------------------------------------------------------------- //
//                            Token class

// A classified symbol in the source language.
//
// Note that tokens internally track their kind as
// an integer value. This allows client languages
// to define their own token enumeration wihtout
// having to instantiate a new token class.
class Token
{
public:
  Token();
  Token(Location, int);
  Token(Location, int, Symbol const*);

  explicit operator bool() const;

  int           kind() const;
  String const& spelling() const;
  Location      location() const;

  Symbol const*         symbol() const;
  Identifier_sym const* identifier_symbol() const;
  Boolean_sym const*    boolean_symbol() const;
  Integer_sym const*    integer_symbol() const;
  Character_sym const*  character_symbol() const;
  String_sym const*     string_symbol() const;

private:
  Location      loc_;
  int           kind_;
  Symbol const* sym_;
};


// Initialize the token to the error token.
inline
Token::Token()
  : Token({}, -1)
{ }


// Initialize a non-symbol token of kind k.
inline
Token::Token(Location loc, int k)
  : Token(loc, k, nullptr)
{ }


// Initialize a token of kind k with the given
// symbol table entry.
inline
Token::Token(Location loc, int k, Symbol const* s)
  : loc_(loc), kind_(k), sym_(s)
{ }


// Returns true if the token is valid (i.e.,
// not the error token).
inline
Token::operator bool() const
{
  return kind_ != -1;
}


// Returns the token kind.
inline int
Token::kind() const
{
  return kind_;
}


// Returns the spelling of the token.
inline String const&
Token::spelling() const
{
  return sym_->spelling();
}


// Returns the source location of the token.
inline Location
Token::location() const
{
  return loc_;
}


// Returns the token's symbol and attributes.
inline Symbol const*
Token::symbol() const
{
  return sym_;
}


// Return the identifier symbol for the token.
inline Identifier_sym const*
Token::identifier_symbol() const
{
  return cast<Identifier_sym>(sym_);
}


// Return the boolean symbol for the token.
inline Boolean_sym const*
Token::boolean_symbol() const
{
  return cast<Boolean_sym>(sym_);
}


// Returns the integer symbol for the token.
inline Integer_sym const*
Token::integer_symbol() const
{
  return cast<Integer_sym>(sym_);
}


// Return the character symbol for the token.
inline Character_sym const*
Token::character_symbol() const
{
  return cast<Character_sym>(sym_);
}


// Return the string symbol for the token.
inline String_sym const*
Token::string_symbol() const
{
  return cast<String_sym>(sym_);
}


// -------------------------------------------------------------------------- //
//                            Token buffer


// A token buffer is a finite sequence of tokens.
//
// Note that tokens are maintained in a linked list
// so that modifications to the buffer don't invalidate
// iterators.
//
// TODO: Define appropriate constructors, etc.
struct Tokenbuf : std::list<Token>
{
  using std::list<Token>::list;
};


// -------------------------------------------------------------------------- //
//                            Token stream


// A token stream provides a stream interface to a
// token buffer.
//
// TODO: This is currently modeling a read/write stream.
// We probably need both a read and write stream position,
// although the write position is always at the end.
class Token_stream
{
public:
  using Position = Tokenbuf::iterator;

  Token_stream();

  bool eof() const;

  Token peek() const;
  Token peek(int) const;
  Token get();
  void put(Token);

  Position position() const;
  Location location() const;

private:
  Tokenbuf buf_;
  Position pos_;
};


// Initialize a token stream with an empty
// buffer.
inline
Token_stream::Token_stream()
  : buf_(), pos_(buf_.begin())
{ }


// Returns true if the stream is at the end of the file.
inline bool
Token_stream::eof() const
{
  return pos_ == buf_.end();
}


// Returns the current token.
inline Token
Token_stream::peek() const
{
  if (eof())
    return Token();
  else
    return *pos_;
}


// Returns the nth token past the current position.
inline Token
Token_stream::peek(int n) const
{
  // Get the nth token, but restore the stream position
  // afterwards. Note that this will gracefully handle
  // an eof during lookahead.
  Position i = pos_;
  while (i != buf_.end() && n) {
    ++i;
    --n;
  }
  if (i == buf_.end())
    return Token();
  else
    return *i;
}


// Returns the current token and advances the stream.
inline Token
Token_stream::get()
{
  if (eof())
    return Token();
  else
    return *pos_++;
}


// Puts the given token at the end of the stream.
inline void
Token_stream::put(Token tok)
{
  buf_.push_back(tok);

  // Make sure that the pos_ isn't pointing past
  // then end after insertion into an empty list.
  if (pos_ == buf_.end())
    pos_ = buf_.begin();
}


// Returns the current position of the stream. This
// effectively an iterator into the buffer.
inline Token_stream::Position
Token_stream::position() const
{
  return pos_;
}


// Returns the source location of the current token.
inline Location
Token_stream::location() const
{
  return peek().location();
}


// -------------------------------------------------------------------------- //
// Symbol initialization

void init_symbols(Symbol_table&);


#endif
