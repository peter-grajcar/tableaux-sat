#ifndef CONNECTIVES_INCLUDED
#define CONNECTIVES_INCLUDED

enum class connective
{
    NOT,
    AND,
    OR,
    IF,
    IFF,
    PAREN_OPEN,
    PAREN_CLOSE,
    UNKNOWN
};

connective char_to_connective(char ch);
int get_precedence(connective conn);
size_t split_index(const std::string &formula);

#endif