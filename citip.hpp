#ifndef __CITIP_HPP__INCLUDED__
#define __CITIP_HPP__INCLUDED__

# include <map>
# include <string>
# include <vector>

# include "parser.hpp"

struct glp_prob;


struct SparseVector
{
    std::map<int, double> entries;
    bool is_equality;

    void inc(int i, double v);
};

typedef std::vector<SparseVector> Matrix;


class ParserOutput : public ParserCallback
{
    int get_var_index(const std::string&);
    int get_set_index(const ast::VarList&);
    void add_quant_vec(SparseVector&, const ast::Quantity&);
    void add_term(SparseVector&, const ast::Term&, double scale=1);

    std::map<std::string, int> vars;

    void add_relation(SparseVector, bool is_inquiry);
public:
    // consider this read-only
    std::vector<int> var_names;

    Matrix inquiries;
    Matrix constraints;

    // parser callback
    void relation(ast::Relation);
    void markov_chain(ast::MarkovChain);
    void mutual_independence(ast::MutualIndependence);
    void function_of(ast::FunctionOf);
};

#endif // include guard
