/*
    This class takes any expression and converts it to its postfix form. It works with mathimatical expressions
    and conditional expressions. Its a bit ridiculous because some of the symbols needed get converted to something
    that won't be part of a variable name, and it isn't really sensible at first glance. The implementation here is
    a bit suspect, and deserves some clean-up but it works. 

    input -> "~(4+2*(9-1))"  | output -> "4 2 9 1 - * + ~"

    The output is a vector of type Postfix::Element so processing it and converting it into ASM / Bytecode will be 
    super simple
*/

#ifndef NHLL_POSTFIX_HPP
#define NHLL_POSTFIX_HPP

#include <string>
#include <vector>

namespace NHLL
{
    //! \brief Infix to postfix converter
    class Postfix
    {
    public:

        //!\brief A Type for a converted element
        enum class Type
        {
            OP, VALUE
        };

        // The postfixer uses its own symbols in a couple of cases for operations
        // That wouldn't make sense to 'outsiders' so these are defined for them to
        // be called by name
        enum class POP
        {
            ADD  = '+',
            MUL  = '*',
            SUB  = '-',
            DIV  = '/',
            POW  = '^',
            MOD  = '%',
            NOT  = '~',
            LSH  = '<',
            RSH  = '>',
            OR   = '|',
            XOR  = '@',
            AND  = '&',
            LTE  = '`',
            GTE  = ';',
            GT   = ',',
            LT   = '#',
            EQ   = '"',
            NE   = '$',
            COR  = '_',
            CAND = ':',
            NONE = ' '
        };

        //!\brief An element of an expression
        struct Element
        {
            std::string value; // Value
            Type type;         // Element type
            POP operation;
        };

        //!\brief Convert an expression to postfix
        //!\param expression The infix expression to conver
        //!\retval Vector of elements. Each element will either be a value (int/double/variable) or an operation (+-/*^)
        //!\note This method ASSUMES that the expression is infix and properly formed. Garbage in will give you garbage out
        std::vector<Element> convert(std::string expression);

    };
}

#endif