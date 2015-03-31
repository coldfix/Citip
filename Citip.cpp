#include <algorithm>    // for_each, ...
#include <iostream>     // cin/cerr etc ...
#include <sstream>      // ostringstream
#include <string>       // getline
#include <vector>       // vector
#include <iterator>     // istream_iterator / back_inserter
#include <functional>   // mem_fn

extern "C" {
    #include "itip1.h"
}


//----------------------------------------
// Compose a string message from multiple values
//----------------------------------------

void print_all(std::ostream& out)
{
    out << std::flush;
}

template <class H, class ...T>
void print_all(std::ostream& out, H head, T... t)
{
    out << head;
    print_all(out, t...);
}

template <class ...T>
std::string sprint_all(T... t)
{
    std::ostringstream out;
    print_all(out, t...);
    return out.str();
}


//----------------------------------------
// Result
//----------------------------------------

struct Result
{
    enum Status {
        True,
        Unknown,
        Error
    };

    Status status;
    std::string message;

    template<class ...T>
    Result(Status s, T... m)
        : status(s)
        , message(sprint_all(m...))
    {
    }
};


//----------------------------------------
// Fun with iteration
//----------------------------------------

namespace detail
{
    class Line : public std::string
    {
        friend std::istream& operator>>(std::istream& in, Line& line)
        {
            return std::getline(in, line);
        }
    };

}

typedef std::istream_iterator<detail::Line> line_iterator;


template <class T, class F>
void inplace_remove_if(T& container, F condition)
{
    container.erase(
            std::remove_if(container.begin(), container.end(), condition),
            container.end());
}

void remove_chars(std::string& str, std::string chars)
{
    auto bad_char = [chars](char c) {
        return chars.find(c) != std::string::npos; };
    inplace_remove_if(str, bad_char);
}

void remove_whitespace(std::string& str)
{
    remove_chars(str, " \t\r\n");
}

void replace_colon(std::string& str)
{
    std::replace(str.begin(), str.end(), ':', ';');
}

std::string quoted(std::string str)
{
    return '"' + str + '"';
}


//----------------------------------------
// invoke ITIP
//----------------------------------------

Result callITIP (std::vector<std::string> expr)
{
    using namespace std;

    for_each(expr.begin(), expr.end(), remove_whitespace);
    for_each(expr.begin(), expr.end(), replace_colon);
    inplace_remove_if(expr, mem_fn(&string::empty));

    if (expr.empty()) {
        return Result(Result::Error, "The information expression is EMPTY!\n You must enter a valid information expression in the first field.");
    }

    int num_expr = expr.size();

    vector<const char*> expr_cstr(num_expr);
    transform(expr.begin(), expr.end(), expr_cstr.begin(),
              mem_fn(&string::c_str));

    int result = itip1(
            const_cast<char**> (expr_cstr.data()),
            num_expr);

    string footnote;
    if (num_expr > 1)
        footnote = "(with the given constraints)";
    else
        footnote = "(without any further constraint)";

    // Success

    if (result == 1) {
        return Result(Result::True,
                "The information expression ", footnote, " is TRUE.");
    }

    // Not solvable

    if (result == 0) {
        return Result(Result::Unknown,
                "The information expression ", footnote, " is Not solvable by Xitip: This implies either of the following situations\n 1.\t The inequality is FALSE, or\n 2.\t This expression is a non-Shannon type inequality which is true.\n \t Currently Xitip is equipped enough to verify only the Shannon type inequalities");
    }

    if (result == -1) {
        return Result(Result::True,
                "You have entered a basic Shannon measure. These measures, without sign are always non negative! Basic Information measures are in the form of entropies (single, joint or conditional) and mutual information(unconditional or conditional). For example the following are basic shannon measures (for 3 random variables X,Y and Z) which are non-nagative by definition\nH(X)>=0, \nH(X,Y)>=0, \nI(X;Y)>=0, \nI(X;Y|Z)>=0,\nH(X|Y)>=0. \nIf you have entered an expression as a single negative constant times these basic Shannon measure, then the inequality is always Non true, beacuase of the non negativity of Shannon measures");
    }

    // Errors

    if (result == -2) {
        return Result(Result::Error,
                "Syntax ERROR: Re-enter the information expression\n\t",
                quoted(expr[0]));
    }

    if (result <= -3) {
        return Result(Result::Error,
                "Syntax ERROR: Constraint ", -result-2, " has wrong syntax.\n"
               " Please re-enter\n\t",
               quoted(expr[-1*result-2]),
               "\n with the correct syntax");
    }

    if (result == 3) {
        return Result(Result::Error,
                "ERROR: The number of distinct random variables cannot be more than 52.");
    }

    if (result == 4){
        return Result(Result::Error,
                "ERROR: Some of the random variables are too long. The maximal length allowed for a single random variable name is 300.");
    }

    if (result == 5) {
        return Result(Result::Error,
                "ERROR: The constraints cannot be inequalities.");
    }

    return Result(Result::Error, "An unexpected error has occurred.");
}


int main (int argc, char *argv[])
{
    using namespace std;

    vector<string> expr;

    bool use_stdin = argc == 1;

    if (string(argv[argc-1]) == "-") {
        --argc;
        use_stdin = true;
    }

    copy(argv+1, argv+argc, back_inserter(expr));

    if (use_stdin) {
        copy(line_iterator(cin), line_iterator(), back_inserter(expr));
    }

    Result r = callITIP(expr);
    cerr << r.message << endl;

    return r.status;
}
