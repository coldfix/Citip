#include <math.h>       // NAN
#include <utility>      // move
#include <sstream>      // istringstream
#include <stdexcept>    // runtime_error

#include <glpk.h>

#include "citip.hpp"
#include "parser.hxx"
#include "scanner.hpp"
#include "common.hpp"

using std::move;
using util::sprint_all;


void check_num_vars(int num_vars)
{
    // The index type (int) must allow to represent column numbers up to
    // 2**num_vars. For signed int MAXINT = 2**(8*sizeof(int)-1)-1,
    // therefore the following is the best we can do (and 30 or so random
    // variables are probably already too much to handle anyway):
    int max_vars = 8*sizeof(int) - 2;
    if (num_vars > max_vars) {
        // Note that the base class destructor ~LinearProblem will still be
        // executed, thus freeing the allocated resource.
        throw std::runtime_error(sprint_all(
                    "Too many variables! At most ", max_vars,
                    " are allowed."));
    }
}


// Shift bits such that the given bit is free.
int skip_bit(int pool, int bit_index)
{
    int bit = 1 << bit_index;
    int left = (pool & ~(bit-1)) << 1;
    int right = pool & (bit-1);
    return left | right;
}


void add_elemental_inequalities(glp_prob* lp, int num_vars)
{
    // NOTE: GLPK uses 1-based indices and never uses the 0th element.
    int indices[5];
    double values[5];
    int i, a, b;

    if (num_vars == 1) {
        indices[1] = 1;
        values[1] = 1;
        int row = glp_add_rows(lp, 1);
        glp_set_row_bnds(lp, row, GLP_LO, 0.0, NAN);
        glp_set_mat_row(lp, row, 1, indices, values);
        return;
    }

    // Identify each variable with its index i from I = {0, 1, ..., N-1}.
    // Then entropy is a real valued set function from the power set of
    // indices P = 2**I. The value for the empty set can be defined to be
    // zero and is irrelevant. Therefore the dimensionality of the problem
    // is 2**N-1.
    int dim = (1<<num_vars) - 1;

    // After choosing 2 variables there are 2**(N-2) possible subsets of
    // the remaining N-2 variables.
    int sub_dim = 1 << (num_vars-2);

    // index of the entropy component corresponding to the joint entropy of
    // all variables. NOTE: since the left-most column is not used, the
    // variables involved in a joint entropy correspond exactly to the bit
    // representation of its index.
    size_t all = dim;

    // Add all elemental conditional entropy positivities, i.e. those of
    // the form H(X_i|X_c)>=0 where c = ~ {i}:
    for (i = 0; i < num_vars; ++i) {
        int c = all ^ (1 << i);
        indices[1] = all;
        indices[2] = c;
        values[1] = +1;
        values[2] = -1;
        int row = glp_add_rows(lp, 1);
        glp_set_row_bnds(lp, row, GLP_LO, 0.0, NAN);
        glp_set_mat_row(lp, row, 2, indices, values);
    }

    // Add all elemental conditional mutual information positivities, i.e.
    // those of the form I(X_a:X_b|X_K)>=0 where a,b not in K
    for (a = 0; a < num_vars-1; ++a) {
        for (b = a+1; b < num_vars; ++b) {
            int A = 1 << a;
            int B = 1 << b;
            for (i = 0; i < sub_dim; ++i) {
                int K = skip_bit(skip_bit(i, a), b);
                indices[1] = A|K;
                indices[2] = B|K;
                indices[3] = A|B|K;
                indices[4] = K;
                values[1] = +1;
                values[2] = +1;
                values[3] = -1;
                values[4] = -1;
                int row = glp_add_rows(lp, 1);
                glp_set_row_bnds(lp, row, GLP_LO, 0.0, NAN);
                glp_set_mat_row(lp, row, K ? 4 : 3, indices, values);
            }
        }
    }
}


//----------------------------------------
// ParserOutput
//----------------------------------------

double SparseVector::get(int i) const
{
    auto&& it = entries.find(i);
    if (it != entries.end())
        return it->second;
    return 0;
}

void SparseVector::inc(int i, double v)
{
    entries[i] += v;
}

void ParserOutput::add_term(SparseVector& v, const ast::Term& t, double scale)
{
    const ast::Quantity& q = t.quantity;
    double coef = scale * t.coefficient;
    int num_parts = q.parts.size();
    if (num_parts == 0) {   // constant
        v.inc(0, coef);
        return;
    }

    // Need to index 2**num_parts subsets. For more detailed reasoning see
    // the check_num_vars() function.
    int max_parts = 8*sizeof(int) - 2;
    if (num_parts > max_parts) {
        throw std::runtime_error(sprint_all(
                    "Too many parts in multivariate mutual information! ",
                    "At most ", max_parts, " are allowed."));
    }

    // Multivariate mutual information is recursively defined by
    //
    //          I(a:…:y:z) = I(a:…:y) - I(a:…:y|z)
    //
    // Here, it is calculated as the alternating sum of (conditional)
    // entropies of all subsets of the parts [Jakulin & Bratko (2003)].
    //
    // See: http://en.wikipedia.org/wiki/Multivariate_mutual_information

    std::vector<int> set_indices(num_parts);
    for (int i = 0; i < num_parts; ++i)
        set_indices[i] = get_set_index(q.parts[i]);

    int num_subsets = 1 << num_parts;
    int c = get_set_index(q.cond);
    // Start at i=1 because i=0 which corresponds to H(empty set) gives no
    // contribution to the sum. Furthermore, the i=0 is already reserved
    // for the constant term for our purposes.
    for (int set = 1; set < num_subsets; ++set) {
        int a = 0;
        int s = -1;
        for (int i = 0; i < num_parts; ++i) {
            if (set & 1<<i) {
                a |= set_indices[i];
                s = -s;
            }
        }
        v.inc(a|c, s*coef);
    }
    if (c)
        v.inc(c, -coef);
}

int ParserOutput::get_var_index(const std::string& s)
{
    auto&& it = vars.find(s);
    if (it != vars.end())
        return it->second;
    int next_index = var_names.size();
    check_num_vars(next_index + 1);
    vars[s] = next_index;
    var_names.push_back(s);
    return next_index;
}

int ParserOutput::get_set_index(const ast::VarList& l)
{
    int idx = 0;
    for (auto&& v : l)
        idx |= 1 << get_var_index(v);
    return idx;
}

void ParserOutput::add_relation(SparseVector v, bool is_inquiry)
{
    if (is_inquiry)
        inquiries.push_back(move(v));
    else
        constraints.push_back(move(v));
}

void ParserOutput::relation(ast::Relation re)
{
    bool is_inquiry = inquiries.empty();
    // create a SparseVector of standard '>=' form. For that the relation
    // needs to be transformed such that:
    //
    //      l <= r      =>      -l + r >= 0
    //      l >= r      =>       l - r >= 0
    //      l  = r      =>       l - r  = 0
    int l_sign = re.relation == ast::REL_LE ? -1 : 1;
    int r_sign = -l_sign;
    SparseVector v;
    v.is_equality = re.relation == ast::REL_EQ;
    for (auto&& term : re.left)
        add_term(v, term, l_sign);
    for (auto&& term : re.right)
        add_term(v, term, r_sign);
    add_relation(v, is_inquiry);
}

void ParserOutput::mutual_independence(ast::MutualIndependence mi)
{
    bool is_inquiry = inquiries.empty();
    // 0 = H(a) + H(b) + H(c) + … - H(a,b,c,…)
    int all = 0;
    SparseVector v;
    v.is_equality = true;
    for (auto&& vl : mi) {
        int idx = get_set_index(vl);
        all |= idx;
        v.inc(idx, 1);
    }
    v.inc(all, -1);
    add_relation(move(v), is_inquiry);
}

void ParserOutput::markov_chain(ast::MarkovChain mc)
{
    bool is_inquiry = inquiries.empty();
    int a = 0;
    for (int i = 0; i+2 < mc.size(); ++i) {
        int b, c;
        a |= get_set_index(mc[i+0]);
        b = get_set_index(mc[i+1]);
        c = get_set_index(mc[i+2]);
        // 0 = I(a:c|b) = H(a|b) + H(c|b) - H(a,c|b)
        SparseVector v;
        v.is_equality = true;
        v.inc(a|b, 1);
        v.inc(c|b, 1);
        v.inc(b, -1);
        v.inc(a|b|c, -1);
        add_relation(move(v), is_inquiry);
    }
}

void ParserOutput::function_of(ast::FunctionOf fo)
{
    bool is_inquiry = inquiries.empty();
    int func = get_set_index(fo.function);
    int of = get_set_index(fo.of);
    // 0 = H(func|of) = H(func,of) - H(of)
    SparseVector v;
    v.is_equality = true;
    v.inc(func|of, 1);
    v.inc(of, -1);
    add_relation(move(v), is_inquiry);
}


//----------------------------------------
// LinearProblem
//----------------------------------------

LinearProblem::LinearProblem()
{
    lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
}

LinearProblem::LinearProblem(int num_cols)
    : LinearProblem()
{
    add_columns(num_cols);
}

LinearProblem::~LinearProblem()
{
    glp_delete_prob(lp);
}

void LinearProblem::add_columns(int num_cols)
{
    glp_add_cols(lp, num_cols);
    for (int i = 1; i <= num_cols; ++i) {
        glp_set_col_bnds(lp, i, GLP_LO, 0, NAN);
    }
}

void LinearProblem::add(const SparseVector& v)
{
    std::vector<int> indices;
    std::vector<double> values;
    indices.reserve(v.entries.size());
    values.reserve(v.entries.size());
    for (auto&& ent : v.entries) {
        if (ent.first == 0)
            continue;
        indices.push_back(ent.first);
        values.push_back(ent.second);
    }

    int kind = v.is_equality ? GLP_FX : GLP_LO;
    int row = glp_add_rows(lp, 1);
    glp_set_row_bnds(lp, row, kind, -v.get(0), NAN);
    glp_set_mat_row(
            lp, row, indices.size(),
            indices.data()-1, values.data()-1);
}

bool LinearProblem::check(const SparseVector& v)
{
    // check for equalities as I>=0 and -I>=0
    if (v.is_equality) {
        SparseVector v2(v);
        v2.is_equality = false;
        if (!check(v2))
            return false;
        for (auto&& ent : v2.entries)
            ent.second = -ent.second;
        return check(v2);
    }

    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev = GLP_MSG_ERR;

    int num_cols = glp_get_num_cols(lp);

    for (int i = 1; i <= num_cols; ++i)
        glp_set_obj_coef(lp, i, v.get(i));

    int outcome = glp_simplex(lp, &parm);
    if (outcome != 0) {
        throw std::runtime_error(sprint_all(
                    "Error in glp_simplex: ", outcome));
    }

    int status = glp_get_status(lp);
    if (status == GLP_OPT) {
        // the original check was for the solution (primal variable values)
        // rather than objective value, but let's do it simpler for now (if
        // an optimum is found, it should be zero anyway):
        return glp_get_obj_val(lp) >= -v.get(0);
    }

    if (status == GLP_UNBND) {
        return false;
    }

    // I am not sure about the exact distinction of GLP_NOFEAS, GLP_INFEAS,
    // GLP_UNDEF, so here is a generic error message:
    throw std::runtime_error(sprint_all(
                "no feasible solution (status code ", status, ")"
                ));
}


ShannonTypeProblem::ShannonTypeProblem(int num_vars)
    : LinearProblem()
{
    check_num_vars(num_vars);
    add_columns((1<<num_vars) - 1);
    add_elemental_inequalities(lp, num_vars);
}


//----------------------------------------
// globals
//----------------------------------------

ParserOutput parse(const std::vector<std::string>& exprs)
{
    ParserOutput out;
    for (int row = 0; row < exprs.size(); ++row) {
        const std::string& line = exprs[row];
        std::istringstream in(line);
        yy::scanner scanner(&in);
        yy::parser parser(&scanner, &out);
        try {
            int result = parser.parse();
            if (result != 0) {
                // Not sure if this can even happen
                throw std::runtime_error("Unknown parsing error");
            }
        }
        catch (yy::parser::syntax_error& e) {
            // For undefined tokens, bison currently just tells us something
            // like 'unexpected $undefined' without printing the offending
            // character. This is much more useful:
            int col = e.location.begin.column;
            int len = 1 + e.location.end.column - col;
            // TODO: The reported location is not entirely satisfying. Any
            // chances for improvement?
            std::string new_message = sprint_all(
                    e.what(), "\n",
                    "in row ", row, " col ", col, ":\n\n"
                    "    ", line, "\n",
                    "    ", std::string(col-1, ' '), std::string(len, '^'));
            throw yy::parser::syntax_error(e.location, new_message);
        }
    }
    if (out.inquiries.empty()) {
        throw std::runtime_error("undefined information expression");
    }
    return move(out);
}


// TODO: implement optimization as in Xitip: collapse variables that only
// appear together

bool check(const ParserOutput& out)
{
    ShannonTypeProblem prob(out.var_names.size());
    for (auto&& constraint : out.constraints)
        prob.add(constraint);
    for (auto&& inquiry : out.inquiries) {
        if (!prob.check(inquiry))
            return false;
    }
    return true;
}
