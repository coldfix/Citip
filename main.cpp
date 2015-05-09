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

#include <iostream>     // cin/cerr etc ...
#include <string>       // getline
#include <vector>       // vector
#include <iterator>     // back_inserter

#include "citip.hpp"
#include "common.hpp"

using util::quoted;
using util::line_iterator;


int main (int argc, char *argv[])
try
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

    bool success = check(parse(expr));

    if (success) {
        cerr << "The information expression is TRUE." << endl;
        return 0;
    }

    cerr << "The information expression is either:\n"
        << "    1. FALSE, or\n"
        << "    2. a non-Shannon type inequality" << endl;
    return 1;
}
catch (std::exception& e)
{
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 2;
}
// force stack unwinding
catch (...)
{
    std::cerr << "UNKNOWN ERROR - aborting." << std::endl;
    return 3;
}
