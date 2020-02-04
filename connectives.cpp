#include "connectives.hpp"
#include <string>

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

size_t get_precedence(connective conn)
{
    switch (conn)
    {
    case connective::NOT:
        return 1;
    case connective::AND:
        return 2;
    case connective::OR:
        return 2;
    case connective::IF:
        return 3;
    case connective::IFF:
        return 3;
    case connective::PAREN_OPEN:
        return 1;
    case connective::PAREN_CLOSE:
        return 1;
    default:
        return 0;
    }
}

size_t split_index(const std::string &formula)
{
}