#include "tableau.hpp"
#include "connectives.hpp"
#include <string>
#include <memory>
#include <queue>

tableau::entry::entry(bool sign, const std::string &subformula, entry *parent)
    : sign(sign), subformula(subformula), parent(parent), left(nullptr), right(nullptr), contradictory(false)
{
}

bool tableau::entry::is_leaf() const
{
    return !left && !right;
}

tableau::tableau(const std::string &formula)
{
    root = std::make_unique<tableau::entry>(false, formula, nullptr);
    to_reduce.push(&*root);
}

void tableau::append(bool sign, const std::string &formula)
{
    //TODO:
}

void tableau::append_atomic(tableau::entry &e, bool sign, connective conn, const std::string &lhs, const std::string &rhs)
{
    switch (conn)
    {
    case connective::NOT:

        break;
    case connective::AND:

        break;
    case connective::OR:

        break;
    case connective::IF:

        break;
    case connective::IFF:

        break;
    default:
        break;
    }
}

void tableau::reduce()
{
    while (!to_reduce.empty())
    {
        tableau::entry &e = *to_reduce.front();
        to_reduce.pop();
        reduce(e);
    }
}

void tableau::reduce(tableau::entry &e)
{
    if (e.reduced || e.contradictory)
        return;

    size_t conn_index = split_index(e.subformula);
    connective conn = char_to_connective(e.subformula[conn_index]);

    std::string lhs = e.subformula.substr(0, conn_index - 1);
    std::string rhs = e.subformula.substr(conn_index + 1, e.subformula.length());

    std::queue<tableau::entry *> entries;
    entries.push(&e);
    while (entries.empty())
    {
        tableau::entry &f = *entries.front();
        entries.pop();
        if (f.is_leaf())
        {
            append_atomic(f, e.sign, conn, lhs, rhs);
        }
        else
        {
            if (f.left)
                entries.push(&*f.left); // !!!
            if (f.right)
                entries.push(&*f.right);
        }
    }

    e.reduced = true;
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