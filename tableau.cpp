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
        e.left = std::make_unique<tableau::entry>(!sign, lhs, &e);
        break;
    case connective::AND:
        if (sign)
        {
            e.left = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &e.left);
        }
        else
        {
            e.left = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.right = std::make_unique<tableau::entry>(sign, rhs, &e);
        }
        break;
    case connective::OR:
        if (sign)
        {
            e.left = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.right = std::make_unique<tableau::entry>(sign, rhs, &e);
        }
        else
        {
            e.left = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &e.left);
        }
        break;
    case connective::IF:
        if (sign)
        {
            e.left = std::make_unique<tableau::entry>(!sign, lhs, &e);
            e.right = std::make_unique<tableau::entry>(sign, rhs, &e);
        }
        else
        {
            e.left = std::make_unique<tableau::entry>(!sign, lhs, &e);
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &e.left);
        }
        break;
    case connective::IFF:
        if (sign)
        {
            e.left = std::make_unique<tableau::entry>(!sign, lhs, &e);
            e.left->left = std::make_unique<tableau::entry>(!sign, rhs, &e.left);
            e.right = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.right->left = std::make_unique<tableau::entry>(sign, rhs, &e.right);
        }
        else
        {
            e.left = std::make_unique<tableau::entry>(!sign, lhs, &e);
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &e.left);
            e.right = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.right->left = std::make_unique<tableau::entry>(!sign, rhs, &e.right);
        }
        break;
    default:
        break;
    }

    // Add newly created entries to the queue for reduction
    if (e.left)
    {
        to_reduce.push(&*e.left);
        if (e.left->left)
            to_reduce.push(&*e.left->left);
    }
    if (e.right)
    {
        to_reduce.push(&*e.right);
        if (e.right->left)
            to_reduce.push(&*e.right->left);
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