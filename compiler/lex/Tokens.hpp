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
        
        INT_DECL,   // int: VARIABLE
        REAL_DECL,  // real: VARIABLE
        

        FUNC_DEF,   // def
        FUNC_NAME,  // def 'functionName'
        FUNC_PARAM, // def functionName( 'param' 'param' 'param' )
        L_PAREN,    // (
        R_PAREN,    // )
        OP_ADD,     // +
        OP_ADDEQ,   // +=
        OP_SUB,     // -
        OP_SUBEQ,   // -=
        OP_MUL,     // *
        OP_MULEQ,   // *=
        OP_DIV,     // /
        OP_DIVEQ,   // /=
        OP_ASSIGN,  // =
        OP_COMPARE, // ==

    };

    struct TokenValue
    {
        Token       token;
        std::string value;
    };
}

#endif