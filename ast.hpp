#ifndef __AST_HPP__INCLUDED__
#define __AST_HPP__INCLUDED__

# include <string>
# include <vector>


namespace ast
{

    enum {
        SIGN_PLUS,
        SIGN_MINUS
    };

    enum {
        REL_EQ,
        REL_LE,
        REL_GE
    };

    typedef std::vector<std::string>    VarList;
    typedef std::vector<VarList>        VarCore;

    struct Quantity
    {
        VarCore parts;
        VarList cond;
    };

    struct Term
    {
        double coefficient;
        Quantity quantity;

        inline Term& flip_sign(int s)
        {
            if (s == SIGN_MINUS) {
                coefficient = -coefficient;
            }
            return *this;
        }
    };

    typedef std::vector<Term> Expression;

    struct Relation {
        Expression left;
        int relation;
        Expression right;
    };

    typedef VarCore MutualIndependence;
    typedef VarCore MarkovChain;

    struct FunctionOf {
        VarList function, of;
    };

}

#endif // include-guard
