#ifndef __SCANNER_HPP__INCLUDED__
#define __SCANNER_HPP__INCLUDED__

#undef yyFlexLexer
#include <FlexLexer.h>
#include "parser.hxx"

// Tell flex which function to define
#ifdef  YY_DECL
# undef YY_DECL
#endif
#define YY_DECL                                 \
    int yy::scanner::lex(                       \
            yy::parser::semantic_type* yylval,  \
            yy::parser::location_type* yylloc)


namespace yy
{
    // To feed data back to bison, the yylex method needs yylval and
    // yylloc parameters. Since the yyFlexLexer class is defined in the
    // system header <FlexLexer.h> the signature of its yylex() method
    // can not be changed anymore. This makes it necessary to derive a
    // scanner class that provides a method with the desired signature:

    class scanner : public yyFlexLexer
    {
    public:
        explicit scanner(std::istream* in=0, std::ostream* out=0);

        int lex(parser::semantic_type* yylval,
                parser::location_type* yylloc);
    };
}

#endif // include guard
