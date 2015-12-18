// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "config.hpp"
#include "beaker/token.hpp"


// TODO: This could be unified with the token so
// that I'd only have to write the spelling once.
char const*
spelling(Token_kind k)
{
  switch (k) {
    case lbrace_tok: return "{";
    case rbrace_tok: return "}";
    case lparen_tok: return "(";
    case rparen_tok: return ")";
    case lbrack_tok: return "[";
    case rbrack_tok: return "]";
    case squote_tok: return "''";
    case dquote_tok: return "\"";
    case comma_tok: return ",";
    case colon_tok: return ":";
    case semicolon_tok: return ";";
    case dot_tok: return ".";
    case equal_tok: return "=";
    case plus_tok: return "+";
    case minus_tok: return "-";
    case star_tok: return "*";
    case slash_tok: return "/";
    case percent_tok: return "%";
    case eq_tok: return "==";
    case ne_tok: return "!=";
    case lt_tok: return "<";
    case gt_tok: return ">";
    case le_tok: return "<=";
    case ge_tok: return ">=";
    case and_tok: return "&&";
    case or_tok: return "||";
    case not_tok: return "!";
    case amp_tok: return "&";
    case arrow_tok: return "->";
    case tilde_tok: return "~";
    case bslash_tok: return "\\";
    case abstract_kw: return "abstract";
    case bool_kw: return "bool";
    case break_kw: return "break";
    case char_kw: return "char";
    case continue_kw: return "continue";
    case def_kw: return "def";
    case double_kw: return "double";
    case else_kw: return "else";
    case float_kw: return "float";
    case foreign_kw: return "else";
    case if_kw: return "if";
    case int16_kw: return "int16";
    case int32_kw: return "int32";
    case int64_kw: return "int64";
    case int_kw: return "int";
    case long_kw: return "long";
    case return_kw: return "return";
    case short_kw: return "short";
    case struct_kw: return "struct";
    case this_kw: return "this";
    case trivial_kw: return "trivial";
    case uint16_kw: return "uint16";
    case uint32_kw: return "uint32";
    case uint64_kw: return "uint64";
    case uint_kw: return "uint";
    case ulong_kw: return "ulong";
    case ushort_kw: return "ushort";
    case var_kw: return "var";
    case virtual_kw: return "virtual";
    case while_kw: return "while";
    default: return "<unspecified>";
  }
}

// Initialize the symbols of the language.
void
init_symbols(Symbol_table& syms)
{
  // Create the symbol table and install all of the
  // default tokens.
  syms.put<Symbol>("{", lbrace_tok);
  syms.put<Symbol>("}", rbrace_tok);
  syms.put<Symbol>("(", lparen_tok);
  syms.put<Symbol>(")", rparen_tok);
  syms.put<Symbol>("[", lbrack_tok);
  syms.put<Symbol>("]", rbrack_tok);
  syms.put<Symbol>("'", squote_tok);
  syms.put<Symbol>("\"", dquote_tok);
  syms.put<Symbol>(",", comma_tok);
  syms.put<Symbol>(":", colon_tok);
  syms.put<Symbol>(";", semicolon_tok);
  syms.put<Symbol>(".", dot_tok);
  syms.put<Symbol>("=", equal_tok);
  syms.put<Symbol>("+", plus_tok);
  syms.put<Symbol>("-", minus_tok);
  syms.put<Symbol>("*", star_tok);
  syms.put<Symbol>("/", slash_tok);
  syms.put<Symbol>("%", percent_tok);
  syms.put<Symbol>("==", eq_tok);
  syms.put<Symbol>("!=", ne_tok);
  syms.put<Symbol>("<", lt_tok);
  syms.put<Symbol>(">", gt_tok);
  syms.put<Symbol>("<=", le_tok);
  syms.put<Symbol>(">=", ge_tok);
  syms.put<Symbol>("&&", and_tok);
  syms.put<Symbol>("||", or_tok);
  syms.put<Symbol>("!", not_tok);
  syms.put<Symbol>("&", amp_tok);
  syms.put<Symbol>("->", arrow_tok);
  syms.put<Symbol>("~", tilde_tok);
  syms.put<Symbol>("\\", bslash_tok);

  // Keywords
  syms.put<Symbol>("abstract", abstract_kw);
  syms.put<Symbol>("bool", bool_kw);
  syms.put<Symbol>("break", break_kw);
  syms.put<Symbol>("char", char_kw);
  syms.put<Symbol>("continue", continue_kw);
  syms.put<Symbol>("def", def_kw);
  syms.put<Symbol>("double", double_kw);
  syms.put<Symbol>("else", else_kw);
  syms.put<Symbol>("float", float_kw);
  syms.put<Symbol>("foreign", foreign_kw);
  syms.put<Symbol>("if", if_kw);
  syms.put<Symbol>("int", int_kw);
  syms.put<Symbol>("int16", int16_kw);
  syms.put<Symbol>("int32", int32_kw);
  syms.put<Symbol>("int64", int64_kw);
  syms.put<Symbol>("long", long_kw);
  syms.put<Symbol>("return", return_kw);
  syms.put<Symbol>("short", short_kw);
  syms.put<Symbol>("struct", struct_kw);
  syms.put<Symbol>("this", this_kw);
  syms.put<Symbol>("trivial", trivial_kw);
  syms.put<Symbol>("uint", uint_kw);
  syms.put<Symbol>("uint16", uint16_kw);
  syms.put<Symbol>("uint32", uint32_kw);
  syms.put<Symbol>("uint64", uint64_kw);
  syms.put<Symbol>("ulong", ulong_kw);
  syms.put<Symbol>("ushort", ushort_kw);
  syms.put<Symbol>("var", var_kw);
  syms.put<Symbol>("virtual", virtual_kw);
  syms.put<Symbol>("while", while_kw);

  // Reserved names
  syms.put<Boolean_sym>("true", boolean_tok, true);
  syms.put<Boolean_sym>("false", boolean_tok, false);

  // Common identifiers
  syms.put<Symbol>("main", identifier_tok);
  syms.put<Symbol>("vptr", identifier_tok);
}
