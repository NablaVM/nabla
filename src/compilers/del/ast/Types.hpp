#ifndef DEL_TYPES_HPP
#define DEL_TYPES_HPP

#include <string>

namespace DEL
{
    enum class NodeType
    {
        ROOT,
        ADD,
        SUB,
        DIV,
        MUL,
        MOD,
        POW,
        VAL,
        ID,

        LTE,
        GTE,
        GT ,
        LT ,
        EQ ,
        NE ,
        LSH,
        RSH,
        BW_OR,
        BW_XOR,
        BW_AND,
        BW_NOT,
        OR,
        AND,
        NEGATE,

        RETURN,

        CALL

        /*
            see Analyzer::validate_assignment_ast 
            for why these belong here
             and Analyzer::check_value_is_valid_for_assignment
             for why they might need to be also represented as a ValType

            POINTER
            REFERENCE
            FUNCTION CALL
            STRUCT
        */

    };

    enum class ValType
    {
        INTEGER,    // An actual integer
        REAL,       // An actual real
        CHAR,       // An actual char
        
        STRING,     // Identifiers
        FUNCTION,   // A function declaration
        REQ_CHECK,  // Requires check by analyzer for value type (reassignment)
        NONE        // Nothing
        /*
            STRUCT
        */
    };

    // This maybe should move to another location 
    struct FunctionParam
    {
        FunctionParam(ValType t, std::string id) : type(t), id(id) {}
        ValType type;   // Type
        std::string id; // The param name
    };

    static inline std::string ValType_to_string(ValType v)
    {
        switch(v)
        {
        case ValType::INTEGER       : return "Integer";
        case ValType::REAL          : return "Real";
        case ValType::CHAR          : return "Char";
        case ValType::STRING        : return "String";
        case ValType::FUNCTION      : return "Function";
        case ValType::NONE          : return "None";
        case ValType::REQ_CHECK     : return "RequiresCheck";
        default                     : return "Unknown";
        }
        return "Unknown";
    }
}

#endif