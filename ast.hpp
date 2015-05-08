#ifndef __AST_HPP__INCLUDED__
#define __AST_HPP__INCLUDED__

# include <string>
# include <vector>

# include "shared_ptr.hpp"


namespace ast
{

    enum {
        SIGN_PLUS,
        SIGN_MINUS
    };

    enum {
        REL_EQ,
        REL_LT,
        REL_GT,
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

    struct RelationData {
        Expression left;
        int relation;
        Expression right;
    };

    struct FunctionOfData {
        VarList function, of;
    };


    struct StatementBase
    {
        virtual ~StatementBase() {}
        virtual void add_to_problem(Problem& lp);
    };


    struct Relation : StatementBase
    {
        RelationData data;

        Relation(const RelationData& d) : data(d) {}
    };

    struct MutualIndependence : StatementBase
    {
        VarCore data;

        explicit MutualIndependence(const VarCore& d) : data(d) {}
    };

    struct MarkovChain : StatementBase
    {
        VarCore data;

        explicit MarkovChain(const VarCore& d) : data(d) {}
    };

    struct FunctionOf : StatementBase
    {
        FunctionOfData data;

        FunctionOf(const FunctionOfData& d) : data(d) {}
    };

    typedef util::SharedPtr<StatementBase> Statement;

}

#endif // include-guard
