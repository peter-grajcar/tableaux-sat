#include "tableau.hpp"
#include "connectives.hpp"
#include <string>
#include <memory>
#include <queue>
#include <stack>
#include <iostream>

tableau::entry::entry(bool sign, const std::string &subformula, entry *parent)
    : sign(sign), subformula(subformula), parent(parent), left(nullptr), right(nullptr), contradictory(false), reduced(false)
{
    reduced = is_propositional_letter(subformula);
    contradictory = is_contradictory() || (parent && parent->contradictory);
    if (contradictory)
        propagate_contradiction();
}

bool tableau::entry::is_leaf() const
{
    return !left && !right;
}

bool tableau::entry::is_contradictory() const
{
    tableau::entry *f = parent;
    while (f)
    {
        if (sign != f->sign && subformula == f->subformula)
            return true;
        f = f->parent;
    }
    return false;
}

void tableau::entry::propagate_contradiction()
{
    tableau::entry *f = parent;
    while (f)
    {
        if (f->left && f->right)
        {
            if (f->left->contradictory && f->right->contradictory)
                f->contradictory = true;
            else
                break;
        }
        else if (f->left && !f->right)
        {
            if (f->left->contradictory)
                f->contradictory = true;
            else
                break;
        }
        else
        {
            break;
        }

        f = f->parent;
    }
}

tableau::tableau(bool sign, const std::string &formula)
{
    root = std::make_unique<tableau::entry>(sign, formula, nullptr);
    if (!is_propositional_letter(formula))
        to_reduce.push(&*root);
}

void tableau::append(bool sign, const std::string &formula)
{
    std::queue<tableau::entry *> entries;
    entries.push(&*root);
    while (!entries.empty())
    {
        tableau::entry &f = *entries.front();
        entries.pop();
        if (f.is_leaf())
        {
            f.left = std::make_unique<tableau::entry>(sign, formula, &f);
            to_reduce.push(&*f.left);
        }
        else
        {
            if (f.left)
                entries.push(&*f.left);
            if (f.right)
                entries.push(&*f.right);
        }
    }
}

void tableau::append_atomic(tableau::entry &e, bool sign, connective conn, const std::string &lhs, const std::string &rhs)
{
    switch (conn)
    {
    case connective::NOT:
        e.left = std::make_unique<tableau::entry>(!sign, rhs, &e);
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
        e.left->propagate_contradiction();
        to_reduce.push(&*e.left);
        if (e.left->left)
        {
            e.left->left->propagate_contradiction();
            to_reduce.push(&*e.left->left);
        }
    }
    if (e.right)
    {
        e.right->propagate_contradiction();
        to_reduce.push(&*e.right);
        if (e.right->left)
        {
            e.right->left->propagate_contradiction();
            to_reduce.push(&*e.right->left);
        }
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
    if (e.reduced || e.contradictory || is_propositional_letter(e.subformula))
    {
        e.reduced = true;
        return;
    }

    size_t conn_index = split_index(e.subformula);
    connective conn = char_to_connective(e.subformula[conn_index]);

    // Split the subformula into two
    size_t a, b, c, d;
    size_t len = e.subformula.length();
    a = 0;
    if (e.subformula[0] == '(')
        ++a;
    b = conn_index - a;
    if (e.subformula[b] == ')')
        --b;
    c = conn_index + 1;
    if (e.subformula[c] == '(')
        ++c;
    d = len - c;
    if (e.subformula[c + d - 1] == ')')
        d--;

    std::string lhs(e.subformula, a, b);
    std::string rhs(e.subformula, c, d);

    std::queue<tableau::entry *> entries;
    entries.push(&e);
    while (!entries.empty())
    {
        tableau::entry &f = *entries.front();
        entries.pop();
        if (f.is_leaf() && !f.contradictory)
        {
            append_atomic(f, e.sign, conn, lhs, rhs);
        }
        else
        {
            if (f.left)
                entries.push(&*f.left);
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

tableau::model tableau::get_model() const
{
    tableau::model m;

    if (root->contradictory)
        return m;

    tableau::entry *e = &*root;

    for (;;)
    {
        if (is_propositional_letter(e->subformula))
            m.emplace(e->subformula, e->sign);
        if (e->left && !e->left->contradictory)
            e = &*e->left;
        else if (e->right && !e->right->contradictory)
            e = &*e->right;
        else
            break;
    }

    return m;
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