#include "tableau.hpp"
#include <iostream>
using namespace std;

int main()
{
    tableau t("~(p | q) = (~p & ~q)");

    t.reduce();

    t.dot_output(cout);

    cerr << (t.is_contradictory() ? "UNSATISFIABLE" : "SATISFIABLE") << endl;
    for (auto i : t.get_model())
    {
        cerr << i.first << " = " << i.second << endl;
    }

    return 0;
}