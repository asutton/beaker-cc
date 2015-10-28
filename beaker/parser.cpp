// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "symbol.hpp"
#include "type.hpp"
#include "expr.hpp"
#include "decl.hpp"
#include "stmt.hpp"
#include "error.hpp"

#include <iostream>
#include <sstream>


// Parse a primary expression.
//
//    primary-expr -> literal | identifier | '(' expr ')'
//
//    literal -> integer-literal | boolean-literal
Expr*
Parser::primary_expr()
{
  // FIXME: switch on the token kind.
  if (Token tok = match_if(identifier_tok))
    return on_id(tok);
  if (Token tok = match_if(boolean_tok))
    return on_bool(tok);
  if (Token tok = match_if(integer_tok))
    return on_int(tok);
  if (match_if(lparen_tok)) {
    Expr* e = expr();
    match(rparen_tok);
    return e;
  }

  // FIXME: Is this definitely an error? Or can we
  // actually return nullptr and continue?
  error("expected primary expression");
}


// Parse a postfix expression.
//
//    postfix-expression -> postfix-expression '(' argument-list ')'
//                       -> primary-expression
Expr*
Parser::postfix_expr()
{
  Expr* e1 = primary_expr();
  while (true) {
    if (match_if(lparen_tok)) {
      Expr_seq args;
      while (lookahead() != rparen_tok) {
        args.push_back(expr());
        if (match_if(comma_tok))
          continue;
        else
          break;
      }
      match(rparen_tok);
      e1 = on_call(e1, args);
    } else {
      break;
    }
  }
  return e1;
}

// Parse a unary expression.
//
//    unary-expr -> '+' unary-expr
//                | '-' unary-expr
//                | '!' unary-expr
//                | postfix-expr
Expr*
Parser::unary_expr()
{
  if (match_if(plus_tok)) {
    Expr* e = unary_expr();
    return on_pos(e);
  } else if (match_if(minus_tok)) {
    Expr* e = unary_expr();
    return on_neg(e);
  } else if (match_if(not_tok)) {
    Expr* e = unary_expr();
    return on_not(e);
  } else {
    return postfix_expr();
  }
}


// Parse a multiplicative expression.
//
//    multiplicative-expr -> multiplicative-expr '*' unary-expr
//                         | multiplicative-expr '/' unary-expr
//                         | multiplicative-expr '%' unary-expr
//                         | unary-expr
Expr*
Parser::multiplicative_expr()
{
  Expr* e1 = unary_expr();
  while (true) {
    if (match_if(star_tok)) {
      Expr* e2 = unary_expr();
      e1 = on_mul(e1, e2);
    } else if (match_if(slash_tok)) {
      Expr* e2 = unary_expr();
      e1 = on_div(e1, e2);
    } else if (match_if(percent_tok)) {
      Expr* e2 = unary_expr();
      e1 = on_rem(e1, e2);
    } else {
      break;
    }
  }
  return e1;
}



// Parse an additive expression.
//
//    additive-expr -> additive-expr '*' multiplicative-expr
//                   | additive-expr '/' multiplicative-expr
//                   | multiplicative-expr
Expr*
Parser::additive_expr()
{
  Expr* e1 = multiplicative_expr();
  while (true) {
    if (match_if(plus_tok)) {
      Expr* e2 = multiplicative_expr();
      e1 = on_add(e1, e2);
    } else if (match_if(minus_tok)) {
      Expr* e2 = multiplicative_expr();
      e1 = on_sub(e1, e2);
    } else {
      break;
    }
  }
  return e1;
}


// Parse an additive expression.
//
//    ordering-expr -> ordering-expr '<' additive-expr
//                   | ordering-expr '>' additive-expr
//                   | ordering-expr '<=' additive-expr
//                   | ordering-expr '>=' additive-expr
//                   | additive-expr
Expr*
Parser::ordering_expr()
{
  Expr* e1 = additive_expr();
  while (true) {
    if (match_if(lt_tok)) {
      Expr* e2 = additive_expr();
      e1 = on_lt(e1, e2);
    } else if (match_if(gt_tok)) {
      Expr* e2 = additive_expr();
      e1 = on_gt(e1, e2);
    } else if (match_if(le_tok)) {
      Expr* e2 = additive_expr();
      e1 = on_le(e1, e2);
    } else if (match_if(ge_tok)) {
      Expr* e2 = additive_expr();
      e1 = on_ge(e1, e2);
    } else {
      break;
    }
  }
  return e1;
}


// Parse an equality expression.
//
//    equality-expr -> equality-expr '<' ordering-expr
//                   | equality-expr '>' ordering-expr
//                   | equality-expr '<=' ordering-expr
//                   | equality-expr '>=' ordering-expr
//                   | ordering-expr
Expr*
Parser::equality_expr()
{
  Expr* e1 = ordering_expr();
  while (true) {
    if (match_if(eq_tok)) {
      Expr* e2 = ordering_expr();
      e1 = on_eq(e1, e2);
    } else if (match_if(ne_tok)) {
      Expr* e2 = ordering_expr();
      e1 = on_ne(e1, e2);
    } else {
      break;
    }
  }
  return e1;
}


// Parse a logical and expression.
//
//    logical-and-expr -> logical-and-expr '&&' equality-expr
//                      | equality-expr
Expr*
Parser::logical_and_expr()
{
  Expr* e1 = equality_expr();
  while (true) {
    if (match_if(and_tok)) {
      Expr* e2 = equality_expr();
      e1 = on_and(e1, e2);
    } else {
      break;
    }
  }
  return e1;
}


// Parse an logical or expression.
//
//    logical-or-expr -> logical-or-expr '&&' logical-and-expr
//                     | logical-and-expr
Expr*
Parser::logical_or_expr()
{
  Expr* e1 = logical_and_expr();
  while (true) {
    if (match_if(or_tok)) {
      Expr* e2 = logical_and_expr();
      e1 = on_or(e1, e2);
    } else {
      break;
    }
  }
  return e1;
}


Expr*
Parser::expr()
{
  return logical_or_expr();
}


// -------------------------------------------------------------------------- //
// Type parsing


// Parse a type.
//
//    type -> 'bool' | 'int' | function-type
//
//    function-type -> '(' type-list ')' '->' type
//
//    type-list -> type | type-list ',' type
Type const*
Parser::type()
{
  // bool
  if (match_if(bool_kw))
    return get_boolean_type();

  // int
  else if (match_if(int_kw))
    return get_integer_type();

  // function-type
  else if (match_if(lparen_tok)) {
    Type_seq ts;
    while (true) {
      ts.push_back(type());
      if (match_if(comma_tok))
        continue;
      else
        break;
    }
    match(rparen_tok);
    match(arrow_tok);
    Type const* t = type();
    return get_function_type(ts, t);
  } 

  // error
  //
  // TODO: Make this a little less vague.
  else
    error("invalid type");
}


// -------------------------------------------------------------------------- //
// Declaration parsing


// Parse a variable declaration.
//
//    variable-decl -> 'var' identifier object-type initializer-clause ';'
//
//    initializer-clause -> '=' expr
Decl*
Parser::variable_decl()
{
  require(var_kw);
  Token n = match(identifier_tok);
  
  // object-type
  match(colon_tok);
  Type const* t = type();

  // initializer-clause
  match(equal_tok);
  Expr* e = expr();
  match(semicolon_tok);

  return on_variable_decl(n, t, e);
}


// Parse a function declaration.
//
//    function-decl -> 'def' identifier parameter-clause return-type function-definition
//
//    parameter-clause -> '(' [parameter-list] ')'
//
//    parameter-list -> parameter-decl | parameter-decl ',' parameter-list
//
//    return-type -> '->' type
//
//    function-definition -> block-stmt
Decl*
Parser::function_decl()
{
  require(def_kw);
  Token n = match(identifier_tok);

  // parameter-clause
  Decl_seq parms;
  match(lparen_tok);
  while (lookahead() != rparen_tok) {
    Decl* p = parameter_decl();
    parms.push_back(p);

    if (match_if(comma_tok))
      continue;
    else
      break;
  }
  match(rparen_tok);

  // return-type
  match(arrow_tok);
  Type const* t = type();

  // function-definition.
  Stmt* s = block_stmt();

  return on_function_decl(n, parms, t, s);
}


// Parse a parameter declaration.
//
//    parameter-decl ::= identifier object-type
Decl*
Parser::parameter_decl()
{
  Token n = match(identifier_tok);

  // object-type
  match(colon_tok);
  Type const* t = type();

  return on_parameter_decl(n, t);
}


// Parse a declaration.
//
//    decl -> variable-decl
//          | function-decl
Decl*
Parser::decl()
{
  switch (lookahead()) {
    case var_kw:
      return variable_decl();
    case def_kw:
      return function_decl();
    default:
      // TODO: Is this a recoverable error?
      error("invalid declaration");
  }
}


// -------------------------------------------------------------------------- //
// Statement parsing
//
// NOTE: Every statement parser declares its own terminator,
// either a ; or }, allowing the sequence parser to catch
// errors and consume up through the end of the statement.

Stmt*
Parser::empty_stmt()
{
  require(semicolon_tok);
  return on_empty_stmt();
}


// Parse a block statement.
//
//    block-stmt -> '{' [stmt-seq] '}'
//
//    stmt-seq -> stmt | stmt stmt-seq
Stmt*
Parser::block_stmt()
{
  std::vector<Stmt*> stmts;
  require(lbrace_tok);
  while (lookahead() != rbrace_tok) {
    try {
      Stmt* s = stmt();
      stmts.push_back(s);
    } catch (Translation_error& err) {
      diagnose(err);
      consume_thru(term_);
    }
  }

  // TODO: This may be a generally unrecoverable error.
  term_ = rbrace_tok;
  match(rbrace_tok);
  return on_block_stmt(stmts);
}


// Parse a return statement.
//
//    return-stmt -> 'return' expr ';'
Stmt*
Parser::return_stmt()
{
  term_ = semicolon_tok;
  require(return_kw);
  Expr* e = expr();
  match(semicolon_tok);
  return on_return_stmt(e);
}


// Parse an if statement.
//
//    if-stmt -> 'if' '(' expr ')' stmt
//             | 'if' '(' expr ')' stmt 'else' stmt
//
// Note that the first form is called an "if-then"
// statement and the latter an "if-else" statement.
Stmt*
Parser::if_stmt()
{
  require(if_kw);
  match(lparen_tok);
  Expr* e = expr();
  match(rparen_tok);
  Stmt* b1 = stmt();
  if (match_if(else_kw)) {
    Stmt* b2 = stmt();
    return on_if_else(e, b1, b2);
  } else {
    return on_if_then(e, b1);
  }
}


// Parse a declaration statement.
//
//    declaration-stmt -> decl
Stmt*
Parser::declaration_stmt()
{
  Decl* d = decl();
  return on_declaration_stmt(d);
}


// Parse an expression statement.
//
//    expression-stmt -> expr ';'
Stmt*
Parser::expression_stmt()
{
  term_ = semicolon_tok;
  Expr* e = expr();
  match(semicolon_tok);
  return on_expression_stmt(e);
}


// Parse a statement.
//
//    stmt -> block-stmt
//          | declaration-stmt
//          | expression-stmt
Stmt*
Parser::stmt()
{
  switch (lookahead()) {
    case semicolon_tok:
      return empty_stmt();

    case lbrace_tok:
      return block_stmt();

    case return_kw:
      return return_stmt();

    case if_kw:
      return if_stmt();

    case var_kw: 
    case def_kw:
      return declaration_stmt();
    
    default:
      return expression_stmt();
  }
}


// -------------------------------------------------------------------------- //
// Top level parsing


// Parse a module.
//
//    module -> decl-seq | <empty>
//
//    decl-seq -> decl | decl-seq
//
// TODO: Return an empty module.
Decl*
Parser::module()
{
  Decl_seq decls;
  while (!ts_.eof()) {
    try {
      Decl* d = decl();
      decls.push_back(d);
    } catch (Translation_error& err) {
      diagnose(err);
      consume_thru(term_);
    }
  }
  return on_module_decl(decls);
}


// -------------------------------------------------------------------------- //
// Helper functions

Token
Parser::match(Token_kind k)
{
  if (lookahead() == k)
    return ts_.get();
  
  std::stringstream ss;
  ss << "expected '" << spelling(k) 
     << "' but got '" <<  ts_.peek().spelling() << "'";
  error(ss.str());
}


// If the current token matches k, return the token
// and advance the stream. Otherwise, return an
// invalid token.
//
// Note that invalid tokens evaluate to false.
Token
Parser::match_if(Token_kind k)
{
  if (lookahead() == k)
    return ts_.get();
  else
    return Token();
}


// Require a token of the given kind. Behavior is
// udefined if the token does not match.
Token
Parser::require(Token_kind k)
{
  assert(lookahead() == k);
  return ts_.get();
}


// Returns the current token and advances the
// underlying token stream.
Token
Parser::accept()
{
  return ts_.get();
}


// Consume up to but not including the given token kind.
void
Parser::consume_until(Token_kind k)
{
  while (!ts_.eof() && lookahead() != k)
    ts_.get();
}


// Consume up to and including the given token kind.
void
Parser::consume_thru(Token_kind k)
{
  consume_until(k);
  if (!ts_.eof())
    ts_.get();
}


// Report an error at the current location.
void
Parser::error(char const* msg)
{
  ++errs_;
  throw Syntax_error(ts_.location(), msg);
}


// Report an error at the current location.
void
Parser::error(String const& msg)
{
  ++errs_;
  throw Syntax_error(ts_.location(), msg);
}


// -------------------------------------------------------------------------- //
// Semantic actions

Expr*
Parser::on_id(Token tok)
{
  return new Id_expr(tok.symbol());
}


Expr*
Parser::on_bool(Token tok)
{
  return new Literal_expr(tok.symbol());
}


Expr*
Parser::on_int(Token tok)
{
  return new Literal_expr(tok.symbol());
}


Expr*
Parser::on_add(Expr* e1, Expr* e2)
{
  return new Add_expr(e1, e2);
}


Expr*
Parser::on_sub(Expr* e1, Expr* e2)
{
  return new Sub_expr(e1, e2);
}


Expr*
Parser::on_mul(Expr* e1, Expr* e2)
{
  return new Mul_expr(e1, e2);
}


Expr*
Parser::on_div(Expr* e1, Expr* e2)
{
  return new Div_expr(e1, e2);
}


Expr*
Parser::on_rem(Expr* e1, Expr* e2)
{
  return new Rem_expr(e1, e2);
}


Expr*
Parser::on_neg(Expr* e)
{
  return new Neg_expr(e);
}


Expr*
Parser::on_pos(Expr* e)
{
  return new Pos_expr(e);
}


Expr*
Parser::on_eq(Expr* e1, Expr* e2)
{
  return new Eq_expr(e1, e2);
}


Expr*
Parser::on_ne(Expr* e1, Expr* e2)
{
  return new Ne_expr(e1, e2);
}


Expr*
Parser::on_lt(Expr* e1, Expr* e2)
{
  return new Lt_expr(e1, e2);
}

Expr*
Parser::on_gt(Expr* e1, Expr* e2)
{
  return new Gt_expr(e1, e2);
}


Expr*
Parser::on_le(Expr* e1, Expr* e2)
{
  return new Le_expr(e1, e2);
}


Expr*
Parser::on_ge(Expr* e1, Expr* e2)
{
  return new Ge_expr(e1, e2);
}


Expr*
Parser::on_and(Expr* e1, Expr* e2)
{
  return new And_expr(e1, e2);
}


Expr*
Parser::on_or(Expr* e1, Expr* e2)
{
  return new Or_expr(e1, e2);
}


Expr*
Parser::on_not(Expr* e)
{
  return new Not_expr(e);
}


Expr*
Parser::on_call(Expr* e, Expr_seq const& a)
{
  return new Call_expr(e, a);
}


Decl* 
Parser::on_variable_decl(Token tok, Type const* t, Expr* e)
{
  return new Variable_decl(tok.symbol(), t, e);
}


Decl* 
Parser::on_parameter_decl(Token tok, Type const* t)
{
  return new Parameter_decl(tok.symbol(), t);
}


Decl* 
Parser::on_function_decl(Token tok, Decl_seq const& p, Type const* t, Stmt* b)
{
  Type const* f = get_function_type(p, t);
  return new Function_decl(tok.symbol(), f, p, b);
}


// FIXME: The name of the module should be the name of the
// file, or maybe even the absolute path of the file.
Decl* 
Parser::on_module_decl(Decl_seq const& d)
{
  Symbol const* sym = syms_.get("<input>");
  return new Module_decl(sym, d);
}


Stmt*
Parser::on_empty_stmt()
{
  return new Empty_stmt();
}


Stmt* 
Parser::on_block_stmt(std::vector<Stmt*> const& s)
{
  return new Block_stmt(s);
}


Stmt*
Parser::on_return_stmt(Expr* e)
{
  return new Return_stmt(e);
}


Stmt*
Parser::on_if_then(Expr* e, Stmt* s)
{
  return new If_then_stmt(e, s);
}


Stmt*
Parser::on_if_else(Expr* e, Stmt* s1, Stmt* s2)
{
  return new If_else_stmt(e, s1, s2);
}


Stmt* 
Parser::on_expression_stmt(Expr* e)
{
  return new Expression_stmt(e);
}


Stmt* 
Parser::on_declaration_stmt(Decl* d)
{
  return new Declaration_stmt(d);
}


