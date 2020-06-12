#ifndef DEL_BLOCK_OPERATIONS_HPP
#define DEL_BLOCK_OPERATIONS_HPP

#include "Codeblock.hpp"

namespace DEL
{
namespace CODE
{
    //
    //    A base conditional block
    //
    class Conditional : public Block
    {
    public:
        Conditional(uint64_t label_id, std::string comparison) : Block()
        {
            std::string label = "conditional_check_" + std::to_string(label_id);
            std::string complete = "conditional_complete_" + std::to_string(label_id);
            comparison = comparison + label;

            std::stringstream ss;

            ss << NLT 
               << remove_for_calc << NL 
               << comparison      << NL << NLT
               << "mov" << WS << "r" << REG_CONDITIONAL << WS << "$0" << TAB << "; False" << NL << NLT
               << "jmp" << WS << complete << NL << NL
               << label << ":" << NL << NLT
               << "mov" << WS << "r" << REG_CONDITIONAL << WS << "$1" << TAB << "; True" << NL << NL
               << complete << ":" << NL << NLT
               << "pushw" << WS << CALC_STACK << WS << "r" << REG_CONDITIONAL << TAB << "; Put result into calc stack" << NL;

            code.push_back(ss.str());
        }
    };

    //
    //      Addition
    //
    class Addition : public Block
    {
    public:
        Addition(CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "add.d" : "add";

            std::stringstream ss; 
            ss << NLT << "; <<< ADDITION >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //      Subtraction
    //
    class Subtraction : public Block
    {
    public:
        Subtraction(CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "sub.d" : "sub";

            std::stringstream ss; 
            ss << NLT << "; <<< SUBTRACTION >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //      Division
    //
    class Division : public Block
    {
    public:
        Division(CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "div.d" : "div";

            std::stringstream ss; 
            ss << NLT << "; <<< DIVISION >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //      Multiplication
    //
    class Multiplication : public Block
    {
    public:
        Multiplication(CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "mul.d" : "mul";

            std::stringstream ss; 
            ss << NLT << "; <<< MULTIPLICATION >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //      Right Shift
    //
    class RightShift : public Block
    {
    public:
        RightShift() : Block()
        {
            std::string cmd = "rsh";

            std::stringstream ss; 
            ss << NLT << "; <<< RIGHT SHIFT >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //      Left Shift
    //
    class LeftShift : public Block
    {
    public:
        LeftShift() : Block()
        {
            std::string cmd = "lsh";

            std::stringstream ss; 
            ss << NLT << "; <<< LEFT SHIFT >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };
    
    //
    //      BW Or
    //
    class BwOr : public Block
    {
    public:
        BwOr() : Block()
        {
            std::string cmd = "or";

            std::stringstream ss; 
            ss << NLT << "; <<< BITWISE OR >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //      BW Not
    //
    class BwNot : public Block
    {
    public:
        BwNot() : Block(true)
        {
            std::string cmd = "not";

            std::stringstream ss; 
            ss << NLT << "; <<< BITWISE NOT >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //      BW Xor
    //
    class BwXor : public Block
    {
    public:
        BwXor() : Block()
        {
            std::string cmd = "xor";

            std::stringstream ss; 
            ss << NLT << "; <<< BITWISE XOR >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };
    
    //
    //      BW And
    //
    class BwAnd : public Block
    {
    public:
        BwAnd() : Block()
        {
            std::string cmd = "and";

            std::stringstream ss; 
            ss << NLT << "; <<< BITWISE AND >>> " << NL
               << remove_for_calc
               << NLT
               << cmd << WS << calculate_and_store;

            code.push_back(ss.str());
        }
    };

    //
    //  LTE
    //
    class Lte : public Block
    {
    public: 
        Lte(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "blte.d" : "blte";

            std::stringstream sscmd;
            sscmd << TAB << cmd << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_RHS << WS;

            std::stringstream ss;
            ss << NLT << "; <<< LTE >>>" << NL;

            code.push_back(ss.str());

            Conditional * c = new Conditional(label_id, sscmd.str());

            std::vector<std::string> ccode = c->get_code();

            code.insert(code.end(), ccode.begin(), ccode.end());

            delete c;
        }
    };

    //
    //  LT
    //
    class Lt : public Block
    {
    public: 
        Lt(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "blt.d" : "blt";

            std::stringstream sscmd;
            sscmd << TAB << cmd << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_RHS << WS;

            std::stringstream ss;
            ss << NLT << "; <<< LT >>>" << NL;

            code.push_back(ss.str());

            Conditional * c = new Conditional(label_id, sscmd.str());

            std::vector<std::string> ccode = c->get_code();

            code.insert(code.end(), ccode.begin(), ccode.end());

            delete c;
        }
    };
    
    //
    //  GTE
    //
    class Gte : public Block
    {
    public: 
        Gte(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "bgte.d" : "bgte";

            std::stringstream sscmd;
            sscmd << TAB << cmd << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_RHS << WS;

            std::stringstream ss;
            ss << NLT << "; <<< GTE >>>" << NL;

            code.push_back(ss.str());

            Conditional * c = new Conditional(label_id, sscmd.str());

            std::vector<std::string> ccode = c->get_code();

            code.insert(code.end(), ccode.begin(), ccode.end());

            delete c;
        }
    };
    
    //
    //  GT
    //
    class Gt : public Block
    {
    public: 
        Gt(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "bgt.d" : "bgt";

            std::stringstream sscmd;
            sscmd << TAB << cmd << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_RHS << WS;

            std::stringstream ss;
            ss << NLT << "; <<< GT >>>" << NL;

            code.push_back(ss.str());

            Conditional * c = new Conditional(label_id, sscmd.str());

            std::vector<std::string> ccode = c->get_code();

            code.insert(code.end(), ccode.begin(), ccode.end());

            delete c;
        }
    };
    
    //
    //  EQ
    //
    class Eq : public Block
    {
    public: 
        Eq(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "beq.d" : "beq";

            std::stringstream sscmd;
            sscmd << TAB << cmd << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_RHS << WS;

            std::stringstream ss;
            ss << NLT << "; <<< EQ >>>" << NL;

            code.push_back(ss.str());

            Conditional * c = new Conditional(label_id, sscmd.str());

            std::vector<std::string> ccode = c->get_code();

            code.insert(code.end(), ccode.begin(), ccode.end());

            delete c;
        }
    };
    
    //
    //  NEQ
    //
    class Neq : public Block
    {
    public: 
        Neq(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string cmd = (is_double_variant(classification)) ? "bne.d" : "bne";

            std::stringstream sscmd;
            sscmd << TAB << cmd << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_RHS << WS;

            std::stringstream ss;
            ss << NLT << "; <<< NEQ >>>" << NL;

            code.push_back(ss.str());

            Conditional * c = new Conditional(label_id, sscmd.str());

            std::vector<std::string> ccode = c->get_code();

            code.insert(code.end(), ccode.begin(), ccode.end());

            delete c;
        }
    };

    //
    //  OR
    //
    class Or : public Block
    {
    public:
        Or(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string true_label = "OR_is_true_"    + std::to_string(label_id);
            std::string complete   = "OR_is_complete_" + std::to_string(label_id);
            std::string comparison = (is_double_variant(classification)) ? "bgt.d" : "bgt";

            std::stringstream ss;
            ss << NLT << "; <<< OR >>>" << NL
               << remove_for_calc << NL << NLT
               << "mov" << WS << "r" << REG_COMPARISON << WS << "$0" << TAB << "; Comparison Value" << NL << NLT
               << comparison << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_COMPARISON << WS << true_label << NL << NLT
               << comparison << WS << "r" << REG_ARITH_RHS << WS << "r" << REG_COMPARISON << WS << true_label << NL << NLT
               << "mov" << WS << "r" << REG_ARITH_LHS << WS << "$0" << TAB << "; False" << NL << NLT
               << "jmp" << WS << complete << NL << NL
               << true_label << ":" << NL << NLT
               << "mov" << WS << "r" << REG_ARITH_LHS << WS << "$1" << TAB << "; True" << NL << NL
               << complete << ":" << NL << NLT
               << "pushw" << WS << CALC_STACK << WS << "r" << REG_ARITH_LHS << TAB << "; Put result in calc stack" << NL;

            code.push_back(ss.str());
        }
    };


    //
    //  And
    //
    class And : public Block
    {
    public:
        And(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block()
        {
            std::string first_true  = "AND_first_true_" + std::to_string(label_id);
            std::string second_true = "AND_second_true_" + std::to_string(label_id);
            std::string complete    = "AND_complete_" + std::to_string(label_id);
            std::string comparison = (is_double_variant(classification)) ? "bgt.d" : "bgt";

            std::stringstream ss;
            ss << NLT << "; <<< AND >>> " << NL 
               << remove_for_calc << NL << NLT 

               // Load comparison value
               << "mov" << WS << "r" << REG_COMPARISON << WS << "$0" << TAB << "; Comparison value" << NL << NLT
               
               // Check the lhs
               << comparison << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_COMPARISON << WS << first_true << NL << NLT
               << "mov" << WS << "r" << REG_ARITH_LHS << WS << "$0" << TAB << "; False" << NL << NLT
               << "jmp" << WS << complete << NL << NL

               // Check the rhs
               << first_true << ":" << NL << NLT
               << comparison << WS << "r" << REG_ARITH_RHS << WS << "r" << REG_COMPARISON << WS << second_true << NL << NLT
               << "mov" << WS << "r" << REG_ARITH_LHS << WS << "$0" << TAB << "; False" << NL << NLT
               << "jmp" << WS << complete << NL << NL

               // Both were true, mark true
               << second_true << ":" << NL << NLT
               << "mov" << WS << "r" << REG_ARITH_LHS << WS << "$1" << TAB << "; True" << NL << NL 
               
               // Complete the check
               << complete << ":" << NL << NLT
               << "pushw"  << WS  << CALC_STACK << WS << "r" << REG_ARITH_LHS << TAB << "; Put result into calc stack" << NL;
            
            code.push_back(ss.str());
        }
    };

    //
    //  Negate
    //
    class Negate : public Block
    {
    public:
        Negate(uint64_t label_id, CODEGEN::TYPES::DataClassification classification) : Block(true)
        {
            std::string set_zero = "NEGATE_set_zero_" + std::to_string(label_id);
            std::string set_comp = "NEGATE_complete_" + std::to_string(label_id);
            std::string comparison = (is_double_variant(classification)) ? "bgt.d" : "bgt";

            std::stringstream ss;
            ss << NLT << "; <<< NEGATE >>>" << NL
               << remove_for_calc << NL << NLT
               << "mov" << WS << "r" << REG_COMPARISON << WS << "$0" << TAB << "; Comparison" << NL << NLT
               << comparison << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_COMPARISON << WS << set_zero << NL << NLT
               << "mov" << WS << "r" << REG_ARITH_LHS << WS << "$1" << NLT
               << "jmp" << WS << set_comp << NL << NL
               << set_zero << ":" << NLT
               << "mov" << WS << "r" << REG_ARITH_LHS << WS << "$0" << NL << NL 
               << set_comp << ":" << NLT
               << "pushw" << WS << CALC_STACK << WS << "r" << REG_ARITH_LHS << TAB << "; Push result into calcl stack" << NL;

            code.push_back(ss.str());
        }
    };

    //
    //  BuiltIn
    //
    class BuiltIn : public Block
    {
    public: 
        BuiltIn(std::string title_comment, std::string function_name) : Block()
        {
            std::stringstream ss;

            ss << NLT
               << "; <<< " << title_comment << " >>> "    << NL << NLT
               << bif_remove_for_calc << NL << NLT 
               << "call"  << WS << function_name << TAB << "; Call built-in function " + title_comment << NL << NLT
               << "pushw" << WS << CALC_STACK << WS << "r" << REG_ADDR_RO << TAB << "; Push value on calc stack" << NL;

            code.push_back(ss.str()); 
        }
    };

    //
    //  Call Function
    //
    class Call : public Block
    {
    public:
        Call(CODEGEN::TYPES::CallInstruction * ins) : Block()
        {
            std::stringstream ss;
            ss << NLT 
               << "; <<< CALL >>> " << NL << NLT
               << "call" << WS << ins->function_name << TAB << "; Call function" << NL;

            if(ins->expect_return_value)
            {
                ss << NLT
                   << "; Get result from call " << NL << NLT
                   << "ldw" << WS << "r" << REG_ADDR_RO << WS << "$" << SETTINGS::GS_INDEX_RETURN_SPACE << "(gs)" << NLT
                   << "pushw" << WS << CALC_STACK << WS << "r" << REG_ADDR_RO << TAB << "; Push result to calculation stack" << NL;
            }

            code.push_back(ss.str()); 
        }
    };
}
}


#endif 