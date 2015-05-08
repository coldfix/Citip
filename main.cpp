/*
    Citip -- Information Theoretic Inequality Prover (C++/CLI version)

    Copyright (C) 2015 Thomas Gläßle <t_glaessle@gmx.de>
                       http://github.com/coldfix/Citip

    main.cpp is the main C++ program file of the ITIP CLI frontend.
    The file is based on the Xitip.c from the Xitip program:

    Copyright (C) 2007 Rethnakaran Pulikkoonattu,
                       Etienne Perron,
                       Suhas Diggavi.
                       Information Processing Group,
                       Ecole Polytechnique Federale de Lausanne,
                       EPFL, Switzerland, CH-1005
                       Email: rethnakaran.pulikkoonattu@epfl.ch
                              etienne.perron@epfl.ch
                              suhas.diggavi@epfl.ch
                       http://ipg.epfl.ch
                       http://xitip.epfl.ch
    Dependent utilities:
    The program uses two other softwares
    1) The ITIP software developed by Raymond Yeung
    2) The GLPK (GNU Linear Programming Kit) for linear programming
    The details of the licensing terms of the above mentioned software shall
    be obtained from the respective websites and owners.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>    // for_each, ...
#include <iostream>     // cin/cerr etc ...
#include <sstream>      // ostringstream
#include <string>       // getline
#include <vector>       // vector
#include <iterator>     // istream_iterator / back_inserter
#include <functional>   // mem_fn

#include "citip.hpp"

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
    inplace_remove_if(expr, mem_fn(&string::empty));

    if (expr.empty()) {
        return Result(Result::Error,
                "The information expression is EMPTY!\n You must enter a valid information expression in the first field.");
    }

    bool success = check(parse(expr));

    string footnote;
    if (expr.size() > 1)
        footnote = "(with the given constraints)";
    else
        footnote = "(without any further constraint)";

    // Success

    if (success) {
        return Result(Result::True,
                "The information expression ", footnote, " is TRUE.");
    }

    // Not solvable

    if (success) {
        return Result(Result::Unknown,
                "The information expression ", footnote, " is Not solvable by Xitip: This implies either of the following situations\n 1.\t The inequality is FALSE, or\n 2.\t This expression is a non-Shannon type inequality which is true.\n \t Currently Xitip is equipped enough to verify only the Shannon type inequalities");
    }

    // Errors

    // TEMP
    int result = 0;
    if (result == -2) {
        return Result(Result::Error,
                "Syntax ERROR: Re-enter the information expression\n\t",
                quoted(expr[0]));
    }

    if (result <= -3) {
        return Result(Result::Error,
                "Syntax ERROR: Constraint ", -result-2, " has wrong syntax.\n"
               " Please re-enter\n\t",
               quoted(expr[-result-2]),
               "\n with the correct syntax");
    }

    if (result == 3) {
        return Result(Result::Error,
                "ERROR: The number of distinct random variables cannot be more than 52.");
    }

    if (result == 4) {
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
