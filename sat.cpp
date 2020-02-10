#include "tableau.hpp"
#include <iostream>
using namespace std;

int main()
{
    tableau t(true, "(p | q) & (~q & ~p)");

    t.reduce();

    t.dot_output(cout);

    std::cerr << (t.is_contradictory() ? "UNSATISFIABLE" : "SATISFIABLE") << std::endl;

    return 0;
}