#ifndef NABLA_TOKENS_HPP
#define NABLA_TOKENS_HPP

#include <string>

namespace NABLA
{
    enum class Token
    {
        REAL,       // A double 
        INTEGER,    // An int
        VARIABLE,   // A string with nothing before it, starting with a num, possible containing a number post first char
        FUNC_DEF,   // def
        FUNC_NAME,  // def 'functionName'
        FUNC_PARAM, // def functionName( 'param' 'param' 'param' )
        L_PAREN,    // (
        R_PAREN,    // )
        OP_ADD,     // +
        OP_SUB,     // -
        OP_MUL,     // *
        OP_DIV      // /
    };

    struct TokenValue
    {
        Token       token;
        std::string value;
    };
}

#endif