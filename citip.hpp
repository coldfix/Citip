#ifndef __CITIP_HPP__INCLUDED__
#define __CITIP_HPP__INCLUDED__

# include <map>
# include <string>
# include <vector>

# include "parser.hxx"

struct glp_prob;

// Function that adds all elemental inequalities as rows to a glp_prob.
void add_elemental_inequalities(glp_prob* lp, int num_vars);

struct SparseVector
{
    std::map<int, double> entries;
    bool is_equality;

    double get(int i) const;
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
    std::vector<std::string> var_names;

    Matrix inquiries;
    Matrix constraints;

    // parser callback
    void relation(ast::Relation);
    void markov_chain(ast::MarkovChain);
    void mutual_independence(ast::MutualIndependence);
    void function_of(ast::FunctionOf);
};


class LinearProblem
{
public:
    explicit LinearProblem(int num_cols);
    ~LinearProblem();

    LinearProblem(const LinearProblem&) = delete;
    LinearProblem& operator = (const LinearProblem&) = delete;

    void add(const SparseVector&);
    bool check(const SparseVector&);

protected:
    glp_prob* lp;
};


class ShannonTypeProblem
    : public LinearProblem
{
public:
    explicit ShannonTypeProblem(int num_vars);
};


ParserOutput parse(const std::vector<std::string>&);

bool check(const ParserOutput&);


#endif // include guard
