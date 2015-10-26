// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#ifndef BEAKER_STRING_HPP
#define BEAKER_STRING_HPP

#include <cctype>
#include <cstring>
#include <algorithm>
#include <iosfwd>
#include <string>
#include <stdexcept>


// -------------------------------------------------------------------------- //
//                              Strings

// The string type.
using String = std::string;


// Returns true if c is the horizontal whitespace.
// Note that vertical tabs and carriage returns
// are considered horizontal white space.
inline bool
is_space(char c)
{
  switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\v':
      return true;
    default:
      return false;
  }
}


// Returns true if c is a newline character.
inline bool
is_newline(char c)
{
  return c == '\n';
}


// Returns true if c in the class [01].
inline bool
is_binary_digit(char c)
{
  return (c - '0' < 2);
}


// Returns true if c is in the class [0-9].
inline bool
is_decimal_digit(char c)
{
  return std::isdigit(c);
}


// If is a digit in base d, return n. Otherwise,
// throw a runtime error.
inline int
if_in_base(int n, int b)
{
  if (n < b)
    return n;
  else
    throw std::runtime_error("invalid digit");
}


// Returns the integer value of a character in base b,
// or -1 if the character is not a digit in that base.
inline int
char_to_int(char c, int b)
{
  if ('0' <= c && c <= '9')
    return if_in_base(c - '0', b);
  if ('a' <= c && c <= 'z')
    return if_in_base(c - 'a' + 10, b);
  if ('A' <= c && c <= 'Z')
    return if_in_base(c - 'A' + 10, b);
  return if_in_base(c, 0);
}


// Returns the integer value of the string in [first, last),
// which contains an integer representation in base b. If
// [first, last) contains any characters that are not digits
// in base b, this throws a runtime error.
//
// Note that T must be an integer type and is given as an
// explicit template argument.
template<typename T, typename I>
inline T
string_to_int(I first, I last, int b)
{
  T n = 0;
  while (first != last) {
    n = n * b + char_to_int(*first, b);
    ++first;
  }
  return n;
}


// Returns the integer value of a string containing an
// integer representation in base b. If s contains any
// characters that are not digits in base b, this throws
// a runtime error.
template<typename T>
inline T
string_to_int(String const& s, int b)
{
  return string_to_int<T>(s.begin(), s.end(), b);
}


// -------------------------------------------------------------------------- //
//                          String builder


// The string builder facilitates the caching of characters
// needed to form a string during lexical analysis.
//
// TODO: Allow strings greater than 128 bytes in length.
// Basically, this entails implementing the small-string 
// optimization.
class String_builder
{
  static constexpr int init_size = 128;
public:
  String_builder();

  String str() const;
  String take();

  void put(char c);
  void put(char const*);
  void put(char const*, int n);
  void put(char const*, char const*);

  void clear();
  
private:
  char buf_[init_size];
  int  len_;
};


inline
String_builder::String_builder()
  : len_(0)
{ 
  std::fill(buf_, buf_ + init_size, 0);
}


// FIXME: This should return a string view, but until
// we can efficiently hash-compare a string view against
// a string, it won't matter.
inline String
String_builder::str() const
{
  return String(buf_, buf_ + len_);
}


// Return the string in the builder and then reset it.
inline String
String_builder::take()
{
  String s = str();
  clear();
  return s;
}


inline void
String_builder::put(char c)
{
  if (len_ == init_size)
    throw std::runtime_error("string builder overflow");
  buf_[len_++] = c;
}


inline void
String_builder::put(char const* s)
{
  put(s, std::strlen(s));
}


inline void
String_builder::put(char const* s, int n)
{
  if (len_ + n >= init_size)
    throw std::runtime_error("string builder overflow");
  std::copy_n(s, n, buf_ + len_);
  len_ += n;
}


inline void
String_builder::put(char const* first, char const* last)
{
  put(first, last - first);
}


// Reset the string builder so that it's content
// is empty. 
inline void
String_builder::clear()
{
  std::fill(buf_, buf_ + init_size, 0);
  len_ = 0;
}


// -------------------------------------------------------------------------- //
//                            String buffer


// The string buffer class provides implements a simple 
// string-based buffer for a stream. The string must not 
// have null characters.
class Stringbuf
{
public:
  Stringbuf() = default;
  Stringbuf(String const&);
  Stringbuf(std::istream& is);

  void assign(std::istream& is);

  char const* begin() const;
  char const* end() const;

private:
  String   buf_;
};


// Initialize the sting buffer from a pre-existing
// string. Note that this copies the string.
inline
Stringbuf::Stringbuf(String const& s)
  : buf_(s)
{ }


// Returns an iterator to the beginning of the string
// buffer.
inline char const* 
Stringbuf::begin() const
{ 
  return buf_.c_str(); 
}


// Returns an iterator past the end of the string buffer.
inline char const* 
Stringbuf::end() const
{ 
  return begin() + buf_.size(); 
}


#endif
