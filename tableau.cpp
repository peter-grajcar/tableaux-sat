#include "tableau.hpp"
#include "connectives.hpp"
#include <string>
#include <memory>
#include <queue>
#include <stack>
#include <iostream>

tableau::entry::entry(std::list<tableau::entry> &tree, bool sign, const std::string &subformula, tableau::index parent)
    : tree(tree), sign(sign), subformula(subformula), parent(parent), left(tree.end()), right(tree.end()), contradictory(false), reduced(false)
{
    reduced = is_propositional_letter(subformula);
    contradictory = is_contradictory() || (parent != tree.end() && parent->contradictory);
}

/**
 * Returns true if the entry has no children.
 */
bool tableau::entry::is_leaf() const
{
    return left == tree.end() && right == tree.end();
}

/**
 * Returns true if a path from this entry to the root contains
 * contradiction with this entry.
 */
bool tableau::entry::is_contradictory() const
{
    tableau::index f = parent;
    while (f != tree.end())
    {
        if (sign != f->sign && subformula == f->subformula)
            return true;
        f = f->parent;
    }
    return false;
}

/**
 * Propagates the contradiction property of the entries on a path
 * from this entry to the root of the tableau.
 */
void tableau::entry::propagate_contradiction()
{
    tableau::index f = parent;
    while (f != tree.end())
    {
        if (f->left != tree.end() && f->right != tree.end())
        {
            if (f->left->contradictory && f->right->contradictory)
                f->contradictory = true;
            else
                break;
        }
        else if (f->left != tree.end() && f->right == tree.end())
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

/**
 * Constructs a new tableau with given formula in its root with given
 * sign.
 */
tableau::tableau(bool sign, const std::string &formula)
{
    tree.emplace_back(tree, sign, formula, tree.end());
    root = std::prev(tree.end());

    if (!is_propositional_letter(formula))
        to_reduce.push(root);
}

/**
 * Appends new entries to each non-contradictory branch of the tableau.
 * The entries are defined by sign and formula.
 */
void tableau::append(bool sign, const std::string &formula)
{
    std::queue<tableau::index> entries;
    entries.push(root);
    while (!entries.empty())
    {
        tableau::index f = entries.front();
        entries.pop();
        if (f->is_leaf() && !f->contradictory)
        {
            tree.emplace_back(tree, sign, formula, f);
            f->left = std::prev(tree.end());
            to_reduce.push(f->left);
        }
        else
        {
            if (f->left != tree.end())
                entries.push(f->left);
            if (f->right != tree.end())
                entries.push(f->right);
        }
    }
}

/**
 * Appends an atomic tableau to the entry e. The atomic tableau is chosen by sign and connective.
 * New entries are created using left and right hand side of the connective (lhs, rhs).
 */
void tableau::append_atomic(tableau::index e, bool sign, connective conn, const std::string &lhs, const std::string &rhs)
{
    switch (conn)
    {
    case connective::NOT:
        tree.emplace_back(tree, !sign, rhs, e);
        e->left = std::prev(tree.end());
        break;
    case connective::AND:
        if (sign)
        {
            tree.emplace_back(tree, sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e->left);
            e->left->left = std::prev(tree.end());
        }
        else
        {
            tree.emplace_back(tree, sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e);
            e->right = std::prev(tree.end());
        }
        break;
    case connective::OR:
        if (sign)
        {
            tree.emplace_back(tree, sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e);
            e->right = std::prev(tree.end());
        }
        else
        {
            tree.emplace_back(tree, sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e->left);
            e->left->left = std::prev(tree.end());
        }
        break;
    case connective::IF:
        if (sign)
        {
            tree.emplace_back(tree, !sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e);
            e->right = std::prev(tree.end());
        }
        else
        {
            tree.emplace_back(tree, !sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e->left);
            e->left->left = std::prev(tree.end());
        }
        break;
    case connective::IFF:
        if (sign)
        {
            tree.emplace_back(tree, !sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, !sign, rhs, e->left);
            e->left->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, lhs, e);
            e->right = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e->right);
            e->right->left = std::prev(tree.end());
        }
        else
        {
            tree.emplace_back(tree, !sign, lhs, e);
            e->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, rhs, e->left);
            e->left->left = std::prev(tree.end());
            tree.emplace_back(tree, sign, lhs, e);
            e->right = std::prev(tree.end());
            tree.emplace_back(tree, !sign, rhs, e->right);
            e->right->left = std::prev(tree.end());
        }
        break;
    default:
        break;
    }

    // Add newly created entries to the queue for reduction
    // and propagate the contradiction.
    if (e->left != tree.end())
    {
        e->left->propagate_contradiction();
        to_reduce.push(e->left);
        if (e->left->left != tree.end())
        {
            e->left->left->propagate_contradiction();
            to_reduce.push(e->left->left);
        }
    }
    if (e->right != tree.end())
    {
        e->right->propagate_contradiction();
        to_reduce.push(e->right);
        if (e->right->left != tree.end())
        {
            e->right->left->propagate_contradiction();
            to_reduce.push(e->right->left);
        }
    }
}

/**
 * Reduces the tableau.
 */
void tableau::reduce()
{
    while (!to_reduce.empty())
    {
        tableau::index e = to_reduce.front();
        to_reduce.pop();
        reduce(e);
    }
}

/**
 * Reduces a single entry of the tableau.
 */
void tableau::reduce(tableau::index e)
{
    if (e->reduced || e->contradictory || is_propositional_letter(e->subformula))
    {
        e->reduced = true;
        return;
    }

    size_t conn_index = split_index(e->subformula);
    connective conn = char_to_connective(e->subformula[conn_index]);

    // Split the subformula into two
    size_t a, b, c, d;
    size_t len = e->subformula.length();
    a = 0;
    if (e->subformula[0] == '(')
        ++a;
    b = conn_index - a;
    if (e->subformula[b] == ')')
        --b;
    c = conn_index + 1;
    if (e->subformula[c] == '(')
        ++c;
    d = len - c;
    if (e->subformula[c + d - 1] == ')')
        d--;

    std::string lhs(e->subformula, a, b);
    std::string rhs(e->subformula, c, d);

    std::queue<tableau::index> entries;
    entries.push(e);
    while (!entries.empty())
    {
        tableau::index f = entries.front();
        entries.pop();
        if (f->is_leaf() && !f->contradictory)
        {
            append_atomic(f, e->sign, conn, lhs, rhs);
        }
        else
        {
            if (f->left != tree.end() && !f->left->contradictory)
                entries.push(f->left);
            if (f->right != tree.end() && !f->right->contradictory)
                entries.push(f->right);
        }
    }

    e->reduced = true;
}

/**
 * Returns true if there are no entries to reduce left 
 * in the tableau, i.e. the tableau is finished.
 */
bool tableau::is_finished() const
{
    return to_reduce.empty();
}

/**
 * Returns true if every path from the root to the arbitrary leaf 
 * is cotradictory, i.e. the tableau is contraddictory.
 */
bool tableau::is_contradictory() const
{
    return root != tree.end() && root->contradictory;
}

/**
 * Returns the model. If the tableau is contradictory an empty
 * model will be returned.
 */
tableau::model tableau::get_model() const
{
    tableau::model m;

    if (root->contradictory)
        return m;

    tableau::index e = root;

    for (;;)
    {
        if (is_propositional_letter(e->subformula))
            m.emplace(e->subformula, e->sign);
        if (e->left != tree.end() && !e->left->contradictory)
            e = e->left;
        else if (e->right != tree.end() && !e->right->contradictory)
            e = e->right;
        else
            break;
    }

    return m;
}

/**
 * Outputs the tableau in a dot language format.
 */
void tableau::dot_output(std::ostream &os) const
{
    using namespace std;

    os << "graph tableau {" << endl
       << "\tnode[shape=\"plaintext\", fontname=\"courier\"]" << endl
       << "\tnodesep=0.4" << endl
       << "\tranksep=0.5" << endl;

    stack<tableau::index> entries;
    map<tableau::entry *, size_t> ids;

    size_t id_counter = 1;

    entries.push(root);
    ids.emplace(&*root, id_counter++);
    while (!entries.empty())
    {
        tableau::index e = entries.top();
        entries.pop();

        if (ids.find(&*e) == ids.end())
            ids.emplace(&*e, id_counter++);

        os << endl
           << "\tE" << ids[&*e] << "[label=\"" << *e << "\"]" << endl;

        if (e->left != tree.end())
        {
            if (ids.find(&*e->left) == ids.end())
                ids.emplace(&*e->left, id_counter++);

            os << "\tE" << ids[&*e] << " -- E" << ids[&*e->left] << endl;
            entries.push(e->left);
        }
        if (e->left != tree.end() && e->right != tree.end())
        {
            if (ids.find(&*e->left) == ids.end())
                ids.emplace(&*e->left, id_counter++);
            if (ids.find(&*e->right) == ids.end())
                ids.emplace(&*e->right, id_counter++);

            os << "\t//hidden node to balance the tree" << endl
               << "\tE" << ids[&*e] << "hidden [label=\"\", width=.1, style=invis]" << endl
               << "\t{rank=same;E" << ids[&*e] << "hidden; E" << ids[&*e->left] << "; E" << ids[&*e->right] << '}' << endl
               << "\tE" << ids[&*e] << " -- E" << ids[&*e] << "hidden[style=invis]" << endl;
            /*<< "\tE" << e->left->id << " -- E" << ids[&e] << "hidden --"
               << "\tE" << e->right->id << " [style=invis]" << endl;*/
        }
        if (e->right != tree.end())
        {
            if (ids.find(&*e->right) == ids.end())
                ids.emplace(&*e->right, id_counter++);

            os << "\tE" << ids[&*e] << " -- E" << ids[&*e->right] << endl;
            entries.push(e->right);
        }
    }

    os << '}' << endl;
}