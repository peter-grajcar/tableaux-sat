#include "tableau.hpp"
#include "connectives.hpp"
#include <string>
#include <memory>
#include <queue>
#include <stack>
#include <iostream>

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
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &*e.left);
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
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &*e.left);
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
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &*e.left);
        }
        break;
    case connective::IFF:
        if (sign)
        {
            e.left = std::make_unique<tableau::entry>(!sign, lhs, &e);
            e.left->left = std::make_unique<tableau::entry>(!sign, rhs, &*e.left);
            e.right = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.right->left = std::make_unique<tableau::entry>(sign, rhs, &*e.right);
        }
        else
        {
            e.left = std::make_unique<tableau::entry>(!sign, lhs, &e);
            e.left->left = std::make_unique<tableau::entry>(sign, rhs, &*e.left);
            e.right = std::make_unique<tableau::entry>(sign, lhs, &e);
            e.right->left = std::make_unique<tableau::entry>(!sign, rhs, &*e.right);
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
    while (!entries.empty())
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

void tableau::dot_output(std::ostream &os) const
{
    using namespace std;

    os << "graph tableau {" << endl
       << "\tnode[shape=\"plaintext\", fontname=\"courier\"]" << endl
       << "\tnodesep=0.4" << endl
       << "\tranksep=0.5" << endl;

    stack<const tableau::entry *> entries;
    map<const tableau::entry *, size_t> ids;

    size_t id_counter = 1;

    entries.push(root.get());
    ids.emplace(root.get(), id_counter++);
    while (!entries.empty())
    {
        const tableau::entry *e = entries.top();
        entries.pop();

        if (ids.find(e) == ids.end())
            ids.emplace(e, id_counter++);

        os << endl
           << "\tE" << ids[e] << "[label=\"" << *e << "\"]" << endl;

        if (e->left != nullptr)
        {
            if (ids.find(&*e->left) == ids.end())
                ids.emplace(&*e->left, id_counter++);

            os << "\tE" << ids[e] << " -- E" << ids[&*e->left] << endl;
            entries.push(e->left.get());
        }
        if (e->left != nullptr && e->right != nullptr)
        {
            if (ids.find(&*e->left) == ids.end())
                ids.emplace(&*e->left, id_counter++);
            if (ids.find(&*e->right) == ids.end())
                ids.emplace(&*e->right, id_counter++);

            os << "\t//hidden node to balance the tree" << endl
               << "\tE" << ids[e] << "hidden [label=\"\", width=.1, style=invis]" << endl
               << "\t{rank=same;E" << ids[e] << "hidden; E" << ids[&*e->left] << "; E" << ids[&*e->right] << '}' << endl
               << "\tE" << ids[e] << " -- E" << ids[e] << "hidden[style=invis]" << endl;
            /*<< "\tE" << e->left->id << " -- E" << ids[&e] << "hidden --"
               << "\tE" << e->right->id << " [style=invis]" << endl;*/
        }
        if (e->right != nullptr)
        {
            if (ids.find(&*e->right) == ids.end())
                ids.emplace(&*e->right, id_counter++);

            os << "\tE" << ids[e] << " -- E" << ids[&*e->right] << endl;
            entries.push(e->right.get());
        }
    }

    os << '}' << endl;
}