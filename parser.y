 /*
 * Bison parser definition.
 */

/* C++ parser interface */
%skeleton "lalr1.cc"
%require  "3.0"

%output  "parser.cpp"
%defines "parser.hpp"

%code requires {
    #include "ast.hpp"
    typedef void* yyscan_t;
}

%code {
    #include "scanner.hpp"
    #include <iostream>
    #include <utility>

    using std::move;

    template <class T, class V>
    T&& enlist(T& t, V& v)
    {
        t.push_back(move(v));
        return move(t);
    }

}

%param  {yyscan_t scanner}
%locations
%error-verbose
%define parse.assert

%define api.value.type variant
%define api.token.prefix {T_}

%token                  END     0   "end of file"

%token <std::string>    NAME
%token <double>         NUM
%token <int>            SIGN
                        REL

%type <ast::Statement>              statement
%type <ast::RelationData>           inform_inequ
%type <ast::VarCore>                mutual_indep
%type <ast::VarCore>                markov_chain
%type <ast::FunctionOfData>         determ_depen
%type <ast::Expression>             inform_expr
%type <ast::Term>                   inform_term
%type <ast::Quantity>               inform_quant
%type <ast::Quantity>               entropy
%type <ast::Quantity>               mutual_inf
%type <ast::VarList>                var_list
%type <ast::VarCore>                mut_inf_core;

%start statement

%%

statement    : inform_inequ     { $$.reset(new ast::Relation($1)); }
             | mutual_indep     { $$.reset(new ast::MutualIndependence($1)); }
             | markov_chain     { $$.reset(new ast::MarkovChain($1)); }
             | determ_depen     { $$.reset(new ast::FunctionOf($1)); }
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
             |             SIGN inform_term     { $$ = {{$2.flip_sign($1)}}; }
             |                  inform_term     { $$ = {{$1}}; }
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
    std::cerr << m
            << " at line " << l.begin.line
            << " col " << l.begin.column
            << std::endl;
}
