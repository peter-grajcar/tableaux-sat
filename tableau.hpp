#include "connectives.hpp"
#include <memory>
#include <string>
#include <queue>
#include <map>
#include <list>
#include <iostream>

#ifndef TABLEAU_HPP_INCLUDED
#define TABLEAU_HPP_INCLUDED

class tableau
{
private:
    class entry;

public:
    using index = std::list<entry>::iterator;
    using model = std::map<std::string, bool>;

private:
    class entry
    {
    private:
        friend class tableau;

        std::list<tableau::entry> &tree;
        bool sign;
        bool contradictory;
        bool reduced;
        std::string subformula;
        tableau::index left;
        tableau::index right;
        tableau::index parent;

    public:
        entry(std::list<tableau::entry> &tree, bool sign, const std::string &subformula, tableau::index parent);
        entry(const entry &) = delete;
        entry(entry &&) = delete;
        entry operator=(const entry &) = delete;
        entry operator=(entry &&) = delete;

        bool is_leaf() const;
        bool is_contradictory() const;
        void propagate_contradiction();

        friend std::ostream &operator<<(std::ostream &os, const entry &e)
        {
            return os << (e.sign ? 'T' : 'F') << '(' << e.subformula << ')' << (e.contradictory ? '*' : ' ');
        }
    };

private:
    index root;
    std::list<tableau::entry> tree;
    std::queue<tableau::index> to_reduce;

    void reduce(tableau::index e);
    void append_atomic(tableau::index e, bool sign, connective conn, const std::string &lhs, const std::string &rhs);

public:
    tableau(bool sign, const std::string &formula);
    tableau(const tableau &) = delete;
    tableau(tableau &&) = delete;
    tableau operator=(const tableau &) = delete;
    tableau operator=(tableau &&) = delete;

    void append(bool sign, const std::string &formula);
    void reduce();
    bool is_finished() const;
    bool is_contradictory() const;
    model get_model() const;

    void dot_output(std::ostream &os) const;
};

#endif