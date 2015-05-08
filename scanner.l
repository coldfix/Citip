/*
    Flex scanner (also known as lexer/lexical analyzer or tokenizer) for the
    Citip grammar. The scanner transforms a byte input stream into a stream
    of tokens.
*/
%top {
    #include "parser.hpp"

    typedef yy::parser::semantic_type YYSTYPE;
    typedef yy::parser::location_type YYLTYPE;
}


%{
    #define YIELD_TOKEN(tok, val, type)     yylval->build<type>(val); \
                                            return yy::parser::token::T_##tok;

    #define YY_TXT                  std::string(yytext, yyleng)
    #define YY_NUM                  std::atof(yytext)

    #define INT_TOKEN(tok, val)     YIELD_TOKEN(tok, val, int)
    #define NUM_TOKEN(tok)          YIELD_TOKEN(tok, YY_NUM, double)
    #define STR_TOKEN(tok)          YIELD_TOKEN(tok, YY_TXT, std::string)
    #define LITERAL                 return yytext[0];

    #define YY_USER_ACTION          yylloc->columns(yyleng);

%}

%option     outfile="scanner.cpp"
%option header-file="scanner.hpp"

 /*
    reentrant           generate reentrant (thread-safe) parser
                        this adds the yyscanner argument to all functions
    bison-bridge        add &yylval argument to yylex
    bison-locations     add &yylloc argument to yylex

    8bit                don't fail on 8-bit input characters
    warn                warn about inconsistencies
    nodefault           don't create default echo-all rule
    noyywrap            don't use yywrap() function
 */
%option reentrant
%option bison-bridge
%option bison-locations

%option 8bit warn nodefault
%option noyywrap

%%

%{
    yylloc->step();
%}

 /* do not treat 'I' or 'H' as identifiers if followed by '(' */
I/\(                        LITERAL
H/\(                        LITERAL

[[:alpha:]][[:alnum:]_]*    STR_TOKEN(NAME)

[[:digit:]]+                NUM_TOKEN(NUM)
[[:digit:]]*\.[[:digit:]]+  NUM_TOKEN(NUM)

\+                          INT_TOKEN(SIGN, ast::SIGN_PLUS)
\-                          INT_TOKEN(SIGN, ast::SIGN_MINUS)

=                           INT_TOKEN(REL, ast::REL_EQ)
\<                          INT_TOKEN(REL, ast::REL_LT)
\>                          INT_TOKEN(REL, ast::REL_GT)
\<=                         INT_TOKEN(REL, ast::REL_LE)
\>=                         INT_TOKEN(REL, ast::REL_GE)

#.*                         {/* eat comments */}
[ \t]                       {/* eat whitespace */}


\n                          yylloc->lines(1); LITERAL

                            /* forward everything else, even invalid
                             * tokens - making use of bison's automatic
                             * error messages */
.                           LITERAL

%%
