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
    class entry
    {
    private:
        friend class tableau;

        bool sign;
        bool contradictory;
        bool reduced;
        std::string subformula;
        std::unique_ptr<entry> left;
        std::unique_ptr<entry> right;
        entry *parent;

    public:
        entry(bool sign, const std::string &subformula, entry *parent);
        entry(const entry &e) = delete;
        entry(entry &&e) = delete;
        entry operator=(const entry &e) = delete;
        entry operator=(entry &&e) = delete;

        bool is_leaf() const;
        bool is_contradictory() const;
        void propagate_contradiction();

        friend std::ostream &operator<<(std::ostream &os, const entry &e)
        {
            return os << (e.sign ? 'T' : 'F') << '(' << e.subformula << ')' << (e.contradictory ? '*' : ' ');
        }
    };

private:
    std::unique_ptr<entry> root;
    std::queue<entry *> to_reduce;

    void reduce(entry &e);
    void append_atomic(entry &e, bool sign, connective conn, const std::string &lhs, const std::string &rhs);

public:
    using model = std::map<std::string, bool>;

    tableau(bool sign, const std::string &formula);
    tableau(const tableau &e) = delete;
    tableau(tableau &&e) = delete;
    tableau operator=(const tableau &e) = delete;
    tableau operator=(tableau &&e) = delete;

    void append(bool sign, const std::string &formula);
    void reduce();
    bool is_finished() const;
    bool is_contradictory() const;
    model get_model() const;

    void dot_output(std::ostream &os) const;
};

#endif