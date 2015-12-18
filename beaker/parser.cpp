// Copyright (c) 2015 Andrew Sutton
// All rights reserved

#include "config.hpp"
#include "beaker/parser.hpp"
#include "beaker/symbol.hpp"
#include "beaker/type.hpp"
#include "beaker/expr.hpp"
#include "beaker/decl.hpp"
#include "beaker/stmt.hpp"
#include "beaker/error.hpp"

#include <iostream>
#include <sstream>


// Parse a primary expression.
//
//    primary-expr -> literal | identifier | '(' expr ')'
//
//    literal -> integer-literal
//             | boolean-literal
//             | character-literal
//             | string-literal
Expr*
Parser::primary_expr()
{
  // identifier
  if (Token tok = match_if(identifier_tok))
    return on_id(tok);

  // boolean-literal
  if (Token tok = match_if(boolean_tok))
    return on_bool(tok);

  // integer-literal
  if (Token tok = match_if(integer_tok))
    return on_int(tok);

  // floating-point-literal
  if (Token tok = match_if(floating_tok))
    return on_float(tok);

  // character-literal
  if (Token tok = match_if(character_tok))
    return on_char(tok);

  // string-literal
  if (Token tok = match_if(string_tok))
    return on_str(tok);

  // NOTE NOTE NOTE
  // Lambda additions
  if (lookahead() == f_slash_tok)
    return lambda_expr();

  // paren-expr
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
//                        | postfix-expression '[' expression ']'
//                        | postfix-expression . identifier
//                        | primary-expression
Expr*
Parser::postfix_expr()
{
  Expr* e1 = primary_expr();
  while (true) {
    // dot-expr
    if (match_if(dot_tok)) {
      Token tok = match(identifier_tok);
      Expr* e2 = on_id(tok);
      e1 = on_dot(e1, e2);
    }

    // call-expr
    else if (match_if(lparen_tok)) {
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
    }

    // index-expr
    else if (match_if(lbrack_tok)) {
      Expr* e2 = expr();
      match(rbrack_tok);
      e1 = on_index(e1, e2);
    }

    // anything else
    else {
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

// NOTE NOTE NOTE
// ADD LAMBDA EXPR here

// NOTE ADD LAMDBA PARSER HERE
Expr*
Parser::lambda_expr()
{
  require(f_slash_tok);

  //Match the identifier inserted earlier
  Token n = match(identifier_tok);
  // parameter-clause
  Decl_seq parms;
  match(lparen_tok);
  while (lookahead() != rparen_tok)
  {

    Decl* p = parameter_decl();

    parms.push_back(p);
    if (match_if(comma_tok))
    {
      continue;
    }
    else
      break;
  }
  match(rparen_tok);

  // return-type
  match(arrow_tok);
  Type const* t = type();
  // must be function-definition
  Stmt* s = block_stmt();

  //return a lambda expression
  return on_lambda(n, parms, t, s);
}



Expr*
Parser::expr()
{
  return logical_or_expr();
}


// -------------------------------------------------------------------------- //
// Type parsing


// Parse a primary type.
//
//    primary-type -> 'bool'
//                  | 'int'
//                  | 'char'
//                  | id-type
//                  | function-type
//
//    function-type -> '(' type-list ')' '->' type
//
//    type-list -> type | type-list ',' type
Type const*
Parser::primary_type()
{
  // id-type
  if (Token tok = match_if(identifier_tok))
    return on_id_type(tok);

  // bool
  else if (match_if(bool_kw))
    return get_boolean_type();

  // char
  else if (match_if(char_kw))
    return get_character_type();

  // int
  else if (match_if(int_kw))
    return get_integer_type();

  // uint
  else if (match_if(uint_kw))
    return get_integer_type(false);

  // short
  else if (match_if(short_kw))
    return get_integer_type(16);

  // ushort
  else if (match_if(ushort_kw))
    return get_integer_type(false,16);

  // long
  else if (match_if(long_kw))
    return get_integer_type(64);

  // ulong
  else if (match_if(ulong_kw))
    return get_integer_type(false,64);

  // int16
  else if (match_if(int16_kw))
    return get_integer_type(16);

  // uint16
  else if (match_if(uint16_kw))
    return get_integer_type(false,16);

  // int32
  else if (match_if(int32_kw))
    return get_integer_type();

  // uint32
  else if (match_if(uint32_kw))
    return get_integer_type(false);

  // int64
  else if (match_if(int64_kw))
    return get_integer_type(64);

  // uint64
  else if (match_if(uint64_kw))
    return get_integer_type(false,64);

  // float
  else if (match_if(float_kw))
      return get_float_type();

  // double
  else if (match_if(double_kw))
      return get_double_type();

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
    return on_function_type(ts, t);
  }

  // error
  //
  // TODO: Make this a little less vague.
  else
    error("invalid type");
}


// Parse a postfix type.
//
//    postfix-type -> primary_type
//                    postfix-type '&'
//                    postfix-type '[]'
//                  | postfix-type '[' expr ']'
//
// TODO: Allow prefix type expressions. These should
// bind more tightly than postfix type expressoins.
//
// TODO: Suffix notation will require parens for grouping.
// For example, a reference to an array would be:
//
//    ref (T[N])
//
// We would need to handle function types carefully.
Type const*
Parser::postfix_type()
{
  Type const* t = primary_type();
  while (true) {
    // reference-type
    if (match_if(amp_tok))
      t = on_reference_type(t);

    // array-types
    else if (match_if(lbrack_tok)) {
      if (match_if(rbrack_tok))
        return on_block_type(t);
      Expr* e = expr();
      match(rbrack_tok);
      t = on_array_type(t, e);
    }

    // No postfix operators
    else
      break;
  }
  return t;
}


// Parse a type.
//
//    type -> postfix-type
Type const*
Parser::type()
{
  return postfix_type();
}



// -------------------------------------------------------------------------- //
// Declaration parsing


// Parse a variable declaration.
//
//    variable-decl -> 'var' identifier object-type initializer-clause
//
//    initializer-clause -> ';' | '=' 'trivial' ';' | '=' expr ';'
Decl*
Parser::variable_decl(Specifier spec)
{
  require(var_kw);
  Token n = match(identifier_tok);

  // object-type
  match(colon_tok);
  Type const* t = type();

  // default initialization (var x : T;)
  if (match_if(semicolon_tok))
    return on_variable(spec, n, t);

  // value initialization (var x : T = e;)
  match(equal_tok);
  if (match_if(trivial_kw)) {
    match(semicolon_tok);
    return on_variable(spec, n, t, trivial_kw);
  }

  Expr* e = expr();
  match(semicolon_tok);
  return on_variable(spec, n, t, e);
}


// Parse a function declaration.
//
//    function-decl -> 'def' identifier parameter-clause return-type ';'
//                   | 'def' identifier parameter-clause return-type function-definition
//    parameter-clause -> '(' [parameter-list] ')'
//
//    parameter-list -> parameter-decl | parameter-decl ',' parameter-list
//
//    return-type -> '->' type
//
//    function-definition -> block-stmt
//
// A function declaration may not have a definition.
Decl*
Parser::function_decl(Specifier spec)
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

  // function declaration
  if (match_if(semicolon_tok))
    return on_function(spec, n, parms, t);

  // function-definition.
  Stmt* s = block_stmt();

  return on_function(spec, n, parms, t, s);
}


// Parse a parameter declaration.
//
//    parameter-decl ::= identifier ':' type
//                     | type
Decl*
Parser::parameter_decl()
{
  // specifier-seq
  Specifier spec = specifier_seq();

  // If we have <token> :, then interpret
  // this as a named parameter.
  if (lookahead(1) == colon_tok) {
    Token n = match(identifier_tok);
    match(colon_tok);
    Type const* t = type();
    return on_parameter(spec, n, t);
  }

  // Otherwise, we probably just have a type.
  else {
    Type const* t = type();
    return on_parameter(spec, t);
  }
}


// Parse a record declaration.
//
//    record-decl -> 'struct' identifier record-body
//
//    record-body -> '{' field-seq '}'
//
//    field-seq -> field-seq | field-seq field-seq
Decl*
Parser::record_decl(Specifier spec)
{
  require(struct_kw);
  Token n = match(identifier_tok);
  const Type* t = nullptr;
  // Determine if it is inheriting from a base class
  if (match_if(colon_tok)) {
    // We have a base class
    t = type();
  }

  // record-body and field-seq
  require(lbrace_tok);
  Decl_seq fs, ms;
  while (lookahead() != rbrace_tok) {
    Specifier spec = specifier_seq();
    if (lookahead() == def_kw) {
      Decl* m = method_decl(spec);
      ms.push_back(m);
    } else if (lookahead() == identifier_tok) {
      Decl* f = field_decl(spec);
      fs.push_back(f);
    } else {
      throw Syntax_error(ts_.location(), "invalid member declaration");
    }
  }
  match(rbrace_tok);

  // Need to replace nullptr with base record
  return on_record(spec, n, fs, ms, t);
}


// Parse a field declaration.
//
//    field-decl -> [specifier-seq] identifier object-type
//
// Note that the specifier-seq is parsed above.
Decl*
Parser::field_decl(Specifier spec)
{
  // actual declaration
  Token n = match(identifier_tok);
  match(colon_tok);
  Type const* t = type();
  match(semicolon_tok);
  return on_field(spec, n, t);
}


// Parse a method declaration.
//
//
//    method-decl -> 'def' identifier parameter-clause return-type function-definition
//
// Note that methods must be declared inside
// the class.
//
// TODO: Support out-of-class definitions?
//
// TODO: Support specifiers to modify the "this"
// parameter. Maybe before the return type? Maybe
// as part of the specifiers?
//
// TODO:
//
//    struct R {
//      const def f() -> void { }   // Why not...
Decl*
Parser::method_decl(Specifier spec)
{
  require(def_kw);
  Token n = match(identifier_tok);

  //check for a this_kw
  //do stuff
  //return on_ctor <- reference on_method

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

  return on_method(spec, n, parms, t, s);
}


// Parse a sequence of declaration specifiers.
//
//    specifier-seq -> specifier | specifier-seq specifier
Specifier
Parser::specifier_seq()
{
  Specifier spec = no_spec;
  while (true) {
    if (match_if(foreign_kw))
      spec |= foreign_spec;
    else if (match_if(abstract_kw))
      spec |= abstract_spec;
    else if (match_if(virtual_kw))
      spec |= virtual_spec;
    else
      break;
  }
  return spec;
}


// Parse a declaration.
//
//    decl -> [specifier-seq] entity-decl
//
//    entity-decl -> variable-decl
//                 | function-decl
Decl*
Parser::decl()
{
  // optional specifier-seq
  Specifier spec = specifier_seq();
  // entity-decl
  switch (lookahead()) {
    case var_kw:
      return variable_decl(spec);
    case def_kw:
      return function_decl(spec);
    case f_slash_tok:
      //
    case struct_kw:
      return record_decl(spec);
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
  return on_empty();
}


// Parse a block statement.
//
//    block-stmt -> '{' [stmt-seq] '}'
//
//    stmt-seq -> stmt | stmt stmt-seq
Stmt*
Parser::block_stmt()
{
  Stmt_seq stmts;
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
  return on_block(stmts);
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
  return on_return(e);
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


// Parse a while statement.
//
//    while -> 'while' '(' expr ')' stmt
Stmt*
Parser::while_stmt()
{
  require(while_kw);
  match(lparen_tok);
  Expr* e = expr();
  match(rparen_tok);
  Stmt* s = stmt();
  return on_while(e, s);
}


// Parse a break statement.
//
//    break-stmt -> 'break' ';'
Stmt*
Parser::break_stmt()
{
  require(break_kw);
  match(semicolon_tok);
  return on_break();
}


// Parse a continue statement.
//
//    continue-stmt -> 'continue' ';'
Stmt*
Parser::continue_stmt()
{
  require(continue_kw);
  match(semicolon_tok);
  return on_continue();
}


// Parse a declaration statement.
//
//    declaration-stmt -> decl
Stmt*
Parser::declaration_stmt()
{
  Decl* d = decl();
  return on_declaration(d);
}


// Parse an expression statement.
//
//    expression-stmt -> expr '=' expr ';'
//                     | expr ';'
//
// The first form of an expression is an
// assignment statement.
Stmt*
Parser::expression_stmt()
{
  term_ = semicolon_tok;
  Expr* e1 = expr();
  if (match_if(equal_tok)) {
    Expr* e2 = expr();
    match(semicolon_tok);
    return on_assign(e1, e2);
  } else {
    match(semicolon_tok);
    return on_expression(e1);
  }
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

    case while_kw:
      return while_stmt();

    case break_kw:
      return break_stmt();

    case continue_kw:
      return continue_stmt();

    case var_kw:
    case def_kw:
    case foreign_kw:
    case f_slash_tok:
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
Parser::module(Module_decl* m)
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
  return on_module(m, decls);
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


// Build a placeholder for a type name. Note that
// we can map these to source code locations.
Type const*
Parser::on_id_type(Token tok)
{
  Type const* t = get_id_type(tok.symbol());
  locs_->emplace(t, tok.location());
  return t;
}


// TODO: Ensure that we can actually construt
// a reference-to-T.
Type const*
Parser::on_reference_type(Type const* t)
{
  return get_reference_type(t);
}


// TODO: Ensure that we can actually construt
// a array-of-T.
Type const*
Parser::on_array_type(Type const* t , Expr* n)
{
  return get_array_type(t, n);
}


// TODO: Ensure that we can actually construt
// a array-of-T.
Type const*
Parser::on_block_type(Type const* t)
{
  return get_block_type(t);
}


Type const*
Parser::on_function_type(Type_seq const& ts, Type const* t)
{
  return get_function_type(ts, t);
}


Expr*
Parser::on_id(Token tok)
{
  return init<Id_expr>(tok.location(), tok.symbol());
}


Expr*
Parser::on_bool(Token tok)
{
  Type const* t = get_boolean_type();
  int v = tok.boolean_symbol()->value();
  return init<Literal_expr>(tok.location(), t, v);
}


Expr*
Parser::on_int(Token tok)
{
  Type const* t = get_integer_type(64);
  int64_t v = tok.integer_symbol()->value();
  return init<Literal_expr>(tok.location(), t, v);
}

Expr*
Parser::on_float(Token tok)
{
  Type const* t = get_double_type();
  double v = tok.floating_symbol()->value();
  return init<Literal_expr>(tok.location(), t, v);
}

Expr*
Parser::on_char(Token tok)
{
  Type const* t = get_character_type();
  int v = tok.character_symbol()->value();
  return init<Literal_expr>(tok.location(), t, v);
}


// Build a new string literal. String literals
// are arrays of characters.
Expr*
Parser::on_str(Token tok)
{
  // Build the string value.
  String_sym const* s = tok.string_symbol();
  Array_value v {
     s->value().c_str(),
     s->value().size()
  };

  // Create the extent of the literal array. This is
  // explicitly more than the length of the string,
  // and includes the null character.
  Type const* z = get_integer_type();
  Expr* n = new Literal_expr(z, v.len + 1);

  // Create the array type.
  Type const* c = get_character_type();
  Type const* t = get_array_type(c, n);

  return init<Literal_expr>(tok.location(), t, v);
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


Expr*
Parser::on_index(Expr* e1, Expr* e2)
{
  return new Index_expr(e1, e2);
}


Expr*
Parser::on_dot(Expr* e1, Expr* e2)
{
  return new Dot_expr(e1, e2);
}

// NOTE NOTE NOTE
// ADDITIONS FOR LAMBDAS
//Lambda_expr(Symbol * s, Decl_seq const& d, Type const * t, Stmt* const& b)
Expr *
Parser::on_lambda(Token tok, Decl_seq const& p, Type const* t, Stmt* b)
{
  Type const* f = get_function_type(p, t);
  return init<Lambda_expr>(tok.location(), tok.symbol(), p, f, b);
}

// TODO: Check declaration specifiers. Not every specifier
// makes sense in every combination or for every declaration.
// A foreign parameter is not particularly useful.


Decl*
Parser::on_variable(Specifier spec, Token tok, Type const* t)
{
  Expr* init = new Default_init(t);
  Decl* decl = new Variable_decl(spec, tok.symbol(), t, init);
  locate(decl, tok.location());
  return decl;
}


Decl*
Parser::on_variable(Specifier spec, Token tok, Type const* t, Token_kind tk)
{
  Expr* init = new Trivial_init(t);
  Decl* decl = new Variable_decl(spec, tok.symbol(), t, init);
  locate(decl, tok.location());
  return decl;
}


Decl*
Parser::on_variable(Specifier spec, Token tok, Type const* t, Expr* e)
{
  Expr* init = new Copy_init(t, e);
  Decl* decl = new Variable_decl(spec, tok.symbol(), t, init);
  locate(decl, tok.location());
  return decl;
}


// Create an unnamed parameter.
Decl*
Parser::on_parameter(Specifier spec, Type const* t)
{
  // Create (or get) an empty identifier.
  Symbol const* s = syms_.put<Identifier_sym>("", identifier_tok);
  return new Parameter_decl(spec, s, t);
}


Decl*
Parser::on_parameter(Specifier spec, Token tok, Type const* t)
{
  return new Parameter_decl(spec, tok.symbol(), t);
}


// Create a function with no body. This is a declaration
// but not a definition.
Decl*
Parser::on_function(Specifier spec, Token tok, Decl_seq const& p, Type const* t)
{
  Type const* f = get_function_type(p, t);
  return new Function_decl(spec, tok.symbol(), f, p, nullptr);
}


Decl*
Parser::on_function(Specifier spec, Token tok, Decl_seq const& p, Type const* t, Stmt* b)
{
  Type const* f = get_function_type(p, t);
  Decl* decl = new Function_decl(tok.symbol(), f, p, b);
  locate(decl, tok.location());
  return decl;
}


// NOTE NOTE NOTE


Decl*
Parser::on_record(Specifier spec, Token n, Decl_seq const& fs, Decl_seq const& ms, Type const* base)
{
  Decl* decl = new Record_decl(n.symbol(), fs, ms, base);
  locate(decl, n.location());
  return decl;
}


Decl*
Parser::on_method(Specifier spec, Token tok, Decl_seq const& p, Type const* t, Stmt* b)
{
  Type const* f = get_function_type(p, t);
  Decl* decl = new Method_decl(spec, tok.symbol(), f, p, b);
  locate(decl, tok.location());
  return decl;
}


Decl*
Parser::on_field(Specifier spec, Token n, Type const* t)
{
  Decl* decl = new Field_decl(n.symbol(), t);
  locate(decl, n.location());
  return decl;
}


// Append the parsed declarations to the module.
// This returns the module m.
Decl*
Parser::on_module(Module_decl* m, Decl_seq const& d)
{
  Decl_seq& d0 = m->decls_;
  d0.insert(d0.end(), d.begin(), d.end());
  return m;
}


Stmt*
Parser::on_empty()
{
  return new Empty_stmt();
}


Stmt*
Parser::on_block(std::vector<Stmt*> const& s)
{
  return new Block_stmt(s);
}


Stmt*
Parser::on_assign(Expr* e1, Expr* e2)
{
  return new Assign_stmt(e1, e2);
}


Stmt*
Parser::on_return(Expr* e)
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
Parser::on_while(Expr* c, Stmt* s)
{
  return new While_stmt(c, s);
}


Stmt*
Parser::on_break()
{
  return new Break_stmt();
}


Stmt*
Parser::on_continue()
{
  return new Continue_stmt();
}


Stmt*
Parser::on_expression(Expr* e)
{
  return new Expression_stmt(e);
}


Stmt*
Parser::on_declaration(Decl* d)
{
  return new Declaration_stmt(d);
}
