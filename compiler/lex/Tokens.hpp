#ifndef NABLA_TOKENS_HPP
#define NABLA_TOKENS_HPP

#include <string>

namespace NABLA
{
    enum class Token
    {
        REAL,
        INTEGER,
        L_PAREN,
        R_PAREN,
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV
    };

    struct TokenValue
    {
        Token       token;
        std::string value;
    };
}

#endif