#include "connectives.hpp"
#include <string>
#include <climits>

/**
 * Converts a character to connective. Returns connective::UNKNOWN
 * if the character does not match any connective.
 */
connective char_to_connective(char ch)
{
    switch (ch)
    {
    case '~':
        return connective::NOT;
    case '&':
        return connective::AND;
    case '|':
        return connective::OR;
    case '-':
        return connective::IF;
    case '=':
        return connective::IFF;
    case '(':
        return connective::PAREN_OPEN;
    case ')':
        return connective::PAREN_CLOSE;
    default:
        return connective::UNKNOWN;
    }
}

/**
 * Returns the precedence of given connective.
 */
int get_precedence(connective conn)
{
    switch (conn)
    {
    case connective::NOT:
        return 3;
    case connective::AND:
        return 2;
    case connective::OR:
        return 2;
    case connective::IF:
        return 1;
    case connective::IFF:
        return 1;
    case connective::PAREN_OPEN:
        return 10;
    case connective::PAREN_CLOSE:
        return 10;
    default:
        return 0;
    }
}

/**
 * Returns an index of a connective with least precedence in 
 * given formula.
 */
size_t split_index(const std::string &formula)
{
    size_t min_index = 0;
    int min_prececence = INT_MAX;
    int level = 0;
    for (size_t i = 0; i < formula.length(); ++i)
    {
        connective conn = char_to_connective(formula[i]);
        if (conn == connective::PAREN_OPEN)
        {
            level += 10;
        }
        else if (conn == connective::PAREN_CLOSE)
        {
            level -= 10;
        }
        else if (conn != connective::UNKNOWN)
        {
            int precedence = level + get_precedence(conn);
            if (precedence < min_prececence)
            {
                min_index = i;
                min_prececence = precedence;
            }
        }
    }
    return min_index;
}

/**
 * Returns true if a formula is a valid propositional letter.
 */
bool is_propositional_letter(const std::string &formula)
{
    for (auto i : formula)
        if (!isalnum(i))
            return false;
    return true;
}