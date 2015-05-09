// This is the main C++ program file of the ITIP CLI frontend.

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
