 /*
    Bison parser definition.

    For those who are new to flex/bison:

    This file is transpiled (using 'bison parser.y') to a .cpp file that
    implements the class yy::parser. This class has the job to analyze the
    token stream resulting from repeated invocation of the yylex() function
    and create an AST (abstract syntax tree) of the given string expressions.

    Although the grammar is quite simple and does not require this level of
    parsing power, using bison was a nice getting-to-know exercise and makes
    language easier to extend and the code easier to maintain.
 */

%output  "parser.cxx"
%defines "parser.hxx"

/* C++ parser interface */
%skeleton "lalr1.cc"
%require  "3.0"

/* add parser members (scanner, cb) and yylex parameters (loc, scanner) */
%parse-param  {yy::scanner* scanner} {ParserCallback* cb}
%locations

/* increase usefulness of error messages */
%define parse.error verbose

/* assert correct cleanup of semantic value objects */
%define parse.assert

%define api.value.type variant
%define api.token.prefix {T_}

%token                  END     0   "end of file"

%token <std::string>    NAME
%token <double>         NUM
%token <int>            SIGN
                        REL

%type <ast::Relation>               inform_inequ
%type <ast::VarCore>                mutual_indep
%type <ast::VarCore>                markov_chain
%type <ast::FunctionOf>             determ_depen
%type <ast::Expression>             inform_expr
%type <ast::Term>                   inform_term
%type <ast::Quantity>               inform_quant
%type <ast::Quantity>               entropy
%type <ast::Quantity>               mutual_inf
%type <ast::VarList>                var_list
%type <ast::VarCore>                mut_inf_core;

%start statement


%code requires {
    #include <stdexcept>
    #include <string>

    #include "ast.hpp"
    #include "location.hh"

    namespace yy {
        class scanner;
    };

    // results
    struct ParserCallback {
        virtual void relation(ast::Relation) = 0;
        virtual void markov_chain(ast::MarkovChain) = 0;
        virtual void mutual_independence(ast::MutualIndependence) = 0;
        virtual void function_of(ast::FunctionOf) = 0;
    };
}

%code {
    #include <iostream>     // cerr, endl
    #include <utility>      // move
    #include <string>

    #include "scanner.hpp"

    using std::move;

    #ifdef  yylex
    # undef yylex
    #endif
    #define yylex scanner->lex

    template <class T, class V>
    T&& enlist(T& t, V& v)
    {
        t.push_back(move(v));
        return move(t);
    }
}
%%

    /* deliver output */

statement    : %empty           { /* allow empty (or pure comment) lines */ }
             | inform_inequ     { cb->relation(move($1)); }
             | mutual_indep     { cb->mutual_independence(move($1)); }
             | markov_chain     { cb->markov_chain(move($1)); }
             | determ_depen     { cb->function_of(move($1)); }
             ;

    /* statements */

inform_inequ : inform_expr REL inform_expr       { $$ = {$1, $2, $3}; }
             ;

markov_chain : markov_chain '/' var_list               { $$ = enlist($1, $3); }
             |     var_list '/' var_list '/' var_list  { $$ = {$1, $3, $5}; }
             ;

mutual_indep : mutual_indep '.' var_list         { $$ = enlist($1, $3); }
             |     var_list '.' var_list         { $$ = {$1, $3}; }
             ;

determ_depen : var_list ':' var_list             { $$ = {$1, $3}; }
             ;

    /* building blocks */

inform_expr  : inform_expr SIGN inform_term     { $$ = enlist($1, $3.flip_sign($2)); }
             |             SIGN inform_term     { $$ = {$2.flip_sign($1)}; }
             |                  inform_term     { $$ = {$1}; }
             ;

inform_term  : NUM inform_quant                 { $$ = {$1, $2}; }
             |     inform_quant                 { $$ = { 1, $1}; }
             | NUM                              { $$ = {$1}; }
             ;

inform_quant : entropy                          { $$ = $1; }
             | mutual_inf                       { $$ = $1; }
             ;

entropy      : 'H' '(' var_list              ')'      { $$ = {{$3}}; }
             | 'H' '(' var_list '|' var_list ')'      { $$ = {{$3}, $5}; }
             ;

mutual_inf   : 'I' '(' mut_inf_core              ')'  { $$ = {{$3}}; }
             | 'I' '(' mut_inf_core '|' var_list ')'  { $$ = {{$3}, $5}; }
             ;

mut_inf_core :  mut_inf_core colon var_list     { $$ = enlist($1, $3); }
             |      var_list colon var_list     { $$ = {$1, $3}; }
             ;

colon        : ':'
             | ';'
             ;

var_list     : var_list ',' NAME                { $$ = enlist($1, $3); }
             |              NAME                { $$ = {$1}; }
             ;

%%

void yy::parser::error(const parser::location_type& l, const std::string& m)
{
    throw yy::parser::syntax_error(l, m);
}
