// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "token.hpp"

char const* 
spelling(Token_kind k)
{
  switch (k) {
    case lbrace_tok: return "{";
    case rbrace_tok: return "}";
    case lparen_tok: return "(";
    case rparen_tok: return ")";
    case comma_tok: return ",";
    case colon_tok: return ":";
    case semicolon_tok: return ";";
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
    case arrow_tok: return "->";

    case bool_kw: return "bool";
    case def_kw: return "def";
    case else_kw: return "else";
    case if_kw: return "if";
    case int_kw: return "int";
    case return_kw: return "return";
    case var_kw: return "var";

    default: return "<unspecified>";
  }
}
