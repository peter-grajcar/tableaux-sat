#include "tableau.hpp"
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

void remove_whitespce(string &str);

int main(int argc, char *argv[])
{
    bool theory_mode = false;
    bool dot_output = false;
    bool proof_mode = false;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-T")
            theory_mode = true;
        else if (arg == "--dot" || arg == "-d")
            dot_output = true;
        else if (arg == "--proof" || arg == "-p")
            proof_mode = true;
        else
        {
            cerr << "Usage: tableaux-sat [-Tdp] [--dot] [--proof]";
            return 1;
        }
    }

    string formula;
    getline(cin, formula);
    remove_whitespce(formula);
    tableau t(!proof_mode, formula);

    if (theory_mode)
    {
        string axiom;
        while (getline(cin, axiom))
        {
            remove_whitespce(axiom);
            t.append(true, axiom);
        }
    }

    t.reduce();

    if (dot_output)
    {
        t.dot_output(cout);
    }
    else if (proof_mode)
    {
        cout << (t.is_contradictory() ? "PROVABLE" : "UNPROVABLE") << endl;
    }
    else
    {
        cout << (t.is_contradictory() ? "UNSATISFIABLE" : "SATISFIABLE") << endl;
        for (auto i : t.get_model())
        {
            cout << i.first << " = " << i.second << endl;
        }
    }

    return 0;
}

void remove_whitespce(string &str)
{
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
}