/***
 * 
 ***/
#include <memory>
#include <string>
#include <queue>
#include <map>
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
        std::string subformula;
        std::unique_ptr<entry> left;
        std::unique_ptr<entry> right;
        entry *parent;

    public:
        entry(bool sign, const std::string &subformula, entry *parent);
    };

private:
    std::unique_ptr<entry> root;
    std::queue<std::unique_ptr<entry>> to_reduce;

    void reduce(entry &e);

public:
    tableau(const std::string &formula);
    void append(bool sign, const std::string &formula);
    void reduce();
    bool is_finished() const;
    bool is_contradictory() const;
    std::map<std::string, bool> model() const;
};

#endif