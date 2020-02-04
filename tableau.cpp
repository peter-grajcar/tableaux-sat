#include "tableau.hpp"
#include <string>
#include <memory>
#include <queue>

tableau::entry::entry(bool sign, const std::string &subformula, entry *parent)
    : sign(sign), subformula(subformula), parent(parent), left(nullptr), right(nullptr), contradictory(false)
{
}

tableau::tableau(const std::string &formula)
{
    root = std::make_unique<tableau::entry>(false, formula, nullptr);
}

void tableau::append(bool sign, const std::string &formula)
{
    //TODO:
}

void tableau::reduce()
{
    //TODO:
}

void tableau::reduce(tableau::entry &e)
{
    //TODO
}

bool tableau::is_finished() const
{
    return to_reduce.empty();
}

bool tableau::is_contradictory() const
{
    return root && root->contradictory;
}

std::map<std::string, bool> tableau::model() const
{
    //TODO:
}