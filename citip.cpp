#include <utility>

#include <glpk.h>

#include "citip.hpp"

extern "C" {
    #include "Citip.h"
}

using std::move;


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
    if (num_parts == 0) {
        // constant
        v.inc(0, -coef);
        return;
    }

    // TODO: raise exception if num_parts too high..

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
    for (int set = 0; set < num_subsets; ++set) {
        int a = 0;
        int s = -1;
        for (int i = 0; i < num_parts; ++i) {
            if (set & 1<<i) {
                a |= set_indices[i];
                s = -s;
            }
        }
        v.inc(a|c, s*coef);
        if (c)
            v.inc(c, -s*coef);
    }
}

int ParserOutput::get_var_index(const std::string& s)
{
    int& index = vars[s];
    if (index == 0)
        index = vars.size() + 1;
    if (index > 8*sizeof(int)) {
        throw std::runtime_error("Too many variables!");
    }
    return index - 1;
}

int ParserOutput::get_set_index(const ast::VarList& l)
{
    int idx = 0;
    for (auto&& v : l)
        idx |= get_var_index(v);
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

LinearProblem::LinearProblem(int num_cols)
{
    lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
    glp_add_cols(lp, num_cols);
    for (int i = 1; i < num_cols; ++i) {
        glp_set_col_bnds(lp, i, GLP_LO, 0, NAN);
    }
}

LinearProblem::~LinearProblem()
{
    glp_delete_prob(lp);
}

void LinearProblem::add(const SparseVector& v)
{
    std::vector<int> indices;
    std::vector<double> coefs;
    indices.reserve(v.entries.size());
    coefs.reserve(v.entries.size());
    for (auto&& ent : v.entries) {
        if (ent.first == 0)
            continue;
        indices.push_back(ent.first);
        indices.push_back(ent.second);
    }

    int kind = v.is_equality ? GLP_FX : GLP_LO;
    int row = glp_add_rows(lp, 1);
    glp_set_row_bnds(lp, row, kind, v.get(0), NAN);
    glp_set_mat_row(
            lp, row, indices.size(),
            indices.data()-1, coefs.data()-1);
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

    for (int i = 1; i < num_cols; ++i)
        glp_set_obj_coef(lp, i, v.get(i));

    int outcome = glp_simplex(lp, &parm);
    if (outcome != 0) {
        // TODO: throw exception
    }
    int status = glp_get_status(lp);

    if (status != GLP_OPT)
        return false;

    // the original check was for the solution (primal variable values)
    // rather than objective value, but let's do it simpler for now:
    // (if an optimum is found, it should be zero anyway)
    return glp_get_obj_val(lp) == 0.0;
}


ShannonTypeProblem::ShannonTypeProblem(int num_vars)
    : LinearProblem((1<<num_vars) - 1)
{
    add_elemental_inequalities(lp, num_vars);
}
