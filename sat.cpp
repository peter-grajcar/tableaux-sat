#include "tableau.hpp"
#include <iostream>
using namespace std;

int main()
{
    tableau t("p & q");

    t.reduce();

    t.dot_output(cout);

    return 0;
}