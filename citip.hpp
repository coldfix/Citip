#ifndef __CITIP_HPP__INCLUDED__
#define __CITIP_HPP__INCLUDED__

# include <map>
# include <string>
# include <vector>

# include "parser.hxx"


struct glp_prob;                    // defined in <glpk.h>


struct SparseVector
{
    std::map<int, double> entries;
    bool is_equality;

    double get(int i) const;        // get component i
    void inc(int i, double v);      // increase/decrease component
};


typedef std::vector<SparseVector> Matrix;


// Lightweight C++ wrapper for a GLPK problem (glp_prob*). This manages a
// problem of the form "Is I>=0 valid, subject to the constraints C>=0, and
// X>=0 for all column variables X".
class LinearProblem
{
public:
    LinearProblem();
    explicit LinearProblem(int num_cols);
    ~LinearProblem();

    void add_columns(int num_cols);

    LinearProblem(const LinearProblem&) = delete;
    LinearProblem& operator = (const LinearProblem&) = delete;

    void add(const SparseVector&);      // add a constraint C>=0
    bool check(const SparseVector&);    // check if I>=0 is redundant

protected:
    glp_prob* lp;
};


// Manage a linear programming problem in the context of random variables.
// The system has 2**num_vars-1 random variables which correspond to joint
// entropies of the non-empty subsets of random variables. These quantities
// are indexed in a canonical way, such that the bit-representation of the
// index is in one-to-one correspondence with the subset.
class ShannonTypeProblem
    : public LinearProblem
{
public:
    explicit ShannonTypeProblem(int num_vars);
};

// This is used automatically for a ShannonTypeProblem.
void add_elemental_inequalities(glp_prob* lp, int num_vars);


class ParserOutput : public ParserCallback
{
    int get_var_index(const std::string&);
    int get_set_index(const ast::VarList&);     // as in 'set of variables'
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


ParserOutput parse(const std::vector<std::string>&);

bool check(const ParserOutput&);


#endif // include guard
