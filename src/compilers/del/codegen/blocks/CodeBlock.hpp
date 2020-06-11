/*

    Code generated in Nabla ASM for specific actions. Called by the CodeGen and loaded into
    an object being built. 

    These objects were built from the previous implementation of code written directly into
    the target vector, so in some cases previous implementations are give in case an crops up we 
    can check against previous implementation. 

    The previous implementation comments will be left in until the generated code is tested.
*/

#ifndef DEL_CODE_BLOCK_HPP
#define DEL_CODE_BLOCK_HPP

#include <vector>
#include <string>
#include "SystemSettings.hpp"
#include "CodegenTypes.hpp"
#include <sstream>
#include <libnabla/endian.hpp>
#include <libnabla/util.hpp>
#include <iostream>

namespace DEL
{

namespace CODE
{
    namespace
    {
        /*
            Note : While these are technically configurable, care should be taken if they are updated, as it is 
                   possible that register trampling could occur if they change. There aren't any checks built in
                   to ensure this doesn't happen.
        */
        // Reg where we put stack pointer
        static const int REG_ADDR_SP = 1;

        // Return 
        static const int REG_ADDR_RO = 0;

        // Built in function operands
        static const int REG_BIF_LHS = 1;
        static const int REG_BIF_RHS = 2;

        // Generated operands
        static const int REG_ARITH_LHS = 8;
        static const int REG_ARITH_RHS = 9;

        // Conditional and comparison registers
        static const int REG_CONDITIONAL = 8;
        static const int REG_COMPARISON  = 7;

        // Line placement
        static constexpr char NL[]  = "\n";
        static constexpr char TAB[] = "\t";
        static constexpr char NLT[] = "\n\t";
        static constexpr char WS[]  = " ";
        static constexpr char WST[] = " \t";
        static constexpr char CALC_STACK[] = "ls";
        static constexpr char MEM_STACK[]  = "gs";

        //  Check if something is a double
        //
        static bool is_double_variant(CODEGEN::TYPES::DataClassification c)
        {
            return c == CODEGEN::TYPES::DataClassification::DOUBLE;
        }

        //  Utilize the mov instruction to place a value of any size upto uint64_t in a register
        //
        static std::vector<std::string> load_64_into_r0(uint64_t le_64, std::string comment)
        {
            std::vector<std::string> result;

            result.push_back("\n\t; Load_64_into_r0 for : " + comment + "\n\n");

            // Mov instruction only handles 32-bit signed. So, if it starts to get bigger,
            // we need to dice it into parts
            if(le_64 > 2147483647)
            {
                uint32_t part_0 = (le_64 & 0xFFFF000000000000) >> 48;
                uint32_t part_1 = (le_64 & 0x0000FFFF00000000) >> 32;
                uint32_t part_2 = (le_64 & 0x00000000FFFF0000) >> 16;
                uint32_t part_3 = (le_64 & 0x000000000000FFFF) >> 0;

                result.push_back("\tmov r0 $" + std::to_string(part_0) + "\t ; part_0 \n");
                result.push_back("\tlsh r0 r0 $48\t\n");

                result.push_back("\tmov r1 $" + std::to_string(part_1) + "\t ; part_1 \n");
                result.push_back("\tlsh r1 r1 $32\t\n");
                result.push_back("\tor  r0 r0 r1\t\n");

                result.push_back("\tmov r1 $" + std::to_string(part_2) + "\t ; part_2 \n");
                result.push_back("\tlsh r1 r1 $16\t\n");
                result.push_back("\tor  r0 r0 r1\t\n");

                result.push_back("\tmov r1 $" + std::to_string(part_3) + "\t ; part_3 \n");
                result.push_back("\tor  r0 r0 r1\t\n");
            }
            else
            {
                result.push_back("\tmov r0 $" + std::to_string(static_cast<uint32_t>(le_64)) + "\t ; Data\n");
            }
            return result;
        }
    }

    // A classification of data size
    enum class SizeClassification
    {
        BYTE,
        WORD
    };

    //
    //  A base 'block' that represents a code block. Contains representations of commonly used actions
    //  as well as a storage unit for generated code that a caller can access to get the data
    //
    class Block
    {
    public:

        Block(bool is_unary=false)
        {
            std::stringstream ss; 
            std::stringstream ss1; 
            std::stringstream ss2; 
            if(is_unary)
            {
                ss << NLT << "popw" << WS << "r" << REG_ARITH_LHS << WS << CALC_STACK << WST << "; Calculation - Unary";
                remove_for_calc = ss.str();
                ss.clear();

                ss1 << WS << "r" << REG_ARITH_LHS << WS  << "r" << REG_ARITH_LHS << WST << "; Perform unary operation"
                   << NLT << "pushw" << WS << CALC_STACK << WS << "r" << REG_ARITH_LHS << WST << "; Put result into calc stack";
                calculate_and_store = ss1.str();
            }
            else
            {
                ss << NLT  << "popw" << WS << "r" << REG_ARITH_RHS << WS << CALC_STACK << WST << "; Calculation RHS"
                   << NLT  << "popw" << WS << "r" << REG_ARITH_LHS << WS << CALC_STACK << WST << "; Calculation LHS";
                remove_for_calc = ss.str();

                ss1 << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_LHS << WS << "r" << REG_ARITH_RHS << WST << "; Perform operation"
                   << NLT << "pushw" << WS << CALC_STACK << WS << "r" << REG_ARITH_LHS << WST << "; Put result into calc stack";

                calculate_and_store = ss1.str();
            }

            ss2 << NLT  << "popw" << WS << "r" << REG_BIF_RHS << WS << CALC_STACK << WST << "; Calculation built in function RHS"
                << NLT  << "popw" << WS << "r" << REG_BIF_LHS << WS << CALC_STACK << WST << "; Calculation built in function LHS";
            bif_remove_for_calc = ss2.str();
            ss2.clear();
        }

        std::vector<std::string> get_code() const
        {
            return code;
        }

    protected:
        std::string remove_for_calc;
        std::string calculate_and_store;
        std::string bif_remove_for_calc;

        std::vector<std::string> code;
    };

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

        /*
            Previous implementation : 
        
                    current_function->instructions.push_back("\n\t; <<< OR >>> \n");
                    current_function->instructions.push_back(remove_words_for_calc);
                    current_function->instructions.push_back("\n\tmov r7 $0\t; Comparison value");
                    std::string true_label = "OR_is_true_"    + std::to_string(label_id);
                    std::string complete   = "OR_is_complete_" + std::to_string(label_id);
                    std::string comparison = (command.classification == CODEGEN::TYPES::DataClassification::DOUBLE) ? "bgt.d " : "bgt ";
                    current_function->instructions.push_back("\n\n\t" + comparison + "r8 r7 " + true_label);
                    current_function->instructions.push_back("\n\t" + comparison + "r9 r7 " + true_label);
                    current_function->instructions.push_back("\n\n\tmov r8 $0 ; False");
                    current_function->instructions.push_back("\n\tjmp " + complete + "\n\n");
                    current_function->instructions.push_back(true_label + ":\n");
                    current_function->instructions.push_back("\n\tmov r8 $1 ; True\n\n");
                    current_function->instructions.push_back(complete + ":\n");
                    current_function->instructions.push_back("\n\tpushw ls r8 \t ; Put result into ls\n");
                    label_id++;
        
        */
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

        /*
            Previous implementation : 
        
                current_function->instructions.push_back("\n\t; <<< AND >>> \n");
                current_function->instructions.push_back(remove_words_for_calc);
                current_function->instructions.push_back("\n\tmov r7 $0\t; Comparison value\n\n");
                std::string first_true  = "AND_first_true_" + std::to_string(label_id);
                std::string second_true = "AND_second_true_" + std::to_string(label_id);
                std::string complete    = "AND_complete_" + std::to_string(label_id);
                std::string comparison = (command.classification == CODEGEN::TYPES::DataClassification::DOUBLE) ? "bgt.d " : "bgt ";
                current_function->instructions.push_back("\t" + comparison + "r8 r7 " + first_true + "\n\n");
                current_function->instructions.push_back("\tmov r8 $0\t; False\n\n");
                current_function->instructions.push_back("\tjmp " + complete + "\n\n");
                current_function->instructions.push_back(first_true + ":\n\n");
                current_function->instructions.push_back("\t" + comparison + "r9 r7 " + second_true + "\n\n");
                current_function->instructions.push_back("\tmov r8 $0\t; False\n\n");
                current_function->instructions.push_back("\tjmp " + complete + "\n\n");
                current_function->instructions.push_back(second_true + ":\n\n");
                current_function->instructions.push_back("\tmov r8 $1\n\n");
                current_function->instructions.push_back(complete + ":\n\n");
                current_function->instructions.push_back("\n\tpushw ls r8 \t ; Put result into ls\n");
                label_id++;
        */
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

        /*
            Previous Implementation:

                current_function->instructions.push_back("\n\t; <<< NEGATE >>> \n");
                current_function->instructions.push_back(remove_single_word_for_calc);
                current_function->instructions.push_back("\n\tmov r7 $0\t; Comparison\n\n");

                std::string comparison = (command.classification == CODEGEN::TYPES::DataClassification::DOUBLE) ? "bgt.d " : "bgt ";
                std::string set_zero = "NEGATE_set_zero_" + std::to_string(label_id);
                std::string set_comp = "NEGATE_complete_" + std::to_string(label_id);
                current_function->instructions.push_back("\t" + comparison + " r8 r7 " + set_zero + "\n\n");
                
                current_function->instructions.push_back("\n\tmov r8 $1\n\tjmp " + set_comp + "\n\n");
                current_function->instructions.push_back(set_zero + ":\n\t mov r8 $0\n\n" + set_comp + ":\n\n");
                current_function->instructions.push_back("\n\tpushw ls r8 \t ; Put result into ls\n");
                label_id++;
        
        */

    };

    //
    //  Load Word
    //
    class Load : public Block
    {
    public:
        Load(SizeClassification classification, CODEGEN::TYPES::AddressValueInstruction * ins) : Block()
        {
            std::string title_comment;
            std::string load;
            std::string push;

            if(classification == SizeClassification::BYTE)
            {
                title_comment = "; <<< LOAD BYTE >>> ";
                load = "ldb";
                push = "push";
            }
            else
            {
                title_comment = "; <<< LOAD WORD >>> ";
                load = "ldw";
                push = "pushw";
            }

            code.push_back(std::string(NLT) + title_comment + std::string(NL));

            // Create move instruction
            std::vector<std::string> store_ins = load_64_into_r0(ins->value, "Address of item in expression");
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            std::stringstream ss;

            ss << NLT 
               << "ldw" << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
               << "add" << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
               << load  << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << "(gs)" << TAB << "; Load value of thing for expression" << NL << NLT
               << push  << WS << CALC_STACK << WS << "r" << REG_ADDR_RO << TAB << "; Push value to calc stack" << NL;

            code.push_back(ss.str()); 
        }

        /*
            Previous Implementation:

                CODEGEN::TYPES::AddressValueInstruction * avins = static_cast<CODEGEN::TYPES::AddressValueInstruction*>(ins);

                current_function->instructions.push_back("\n\t; <<< LOAD WORD >>> \n");
                uint64_t word_address = avins->value;

                // Generate a register with the address for the destination
                std::vector<std::string> store_ins = generator.load_64_into_r0(word_address, "Address of thing in expression");
                current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

                // Add the relative address of the item to the start position of the function in memory to acquire the actual destination
                current_function->instructions.push_back("\n\tldw r1 $0(ls)\t; Start of stack frame\n");
                current_function->instructions.push_back("\n\tadd r0 r0 r1\t; Item location in function memory\n");

                // Load GS address at r0 to r0
                current_function->instructions.push_back("\n\tldw r0 r0(gs)\t; Load value of thing for expression\n");
                current_function->instructions.push_back("\tpushw ls r0\t; Push value to local stack for calculation\n");
                break;
        */
    };

    //
    //  Store Byte
    //
    class Store : public Block
    {
    public:
        Store(SizeClassification classification, uint64_t mem_start, std::string id) : Block()
        {
            std::string title_comment;
            std::string pop;
            std::string store;

            std::string address_comment = "Address for [ " + id + " ]";

            if(classification == SizeClassification::BYTE)
            {
                title_comment = "; <<< STORE BYTE >>> ";
                pop = "pop";
                store = "stb";
            }
            else
            {
                title_comment = "; <<< STORE WORD >>> ";
                pop = "popw";
                store = "stw";
            }

            code.push_back(std::string(NLT) + title_comment + std::string(NL));

            // Create move instruction
            std::vector<std::string> store_ins = load_64_into_r0(mem_start, address_comment);
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            std::stringstream ss;
            ss << NLT 
               << "ldw"  << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
               << "add"  << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
               << "; ---- Get Result ---- " << NL << NLT
               << pop << WS << "r" << REG_ARITH_LHS << WS << CALC_STACK << NL << NLT
               << "; ---- Store Result ---- " << NL << NLT
               << store << WS << "r" << REG_ADDR_RO << "(gs)" << WS << "r" << REG_ARITH_LHS << TAB << "; Store in memory" << NL;

            code.push_back(ss.str()); 
        }

        /*
            Previous Implementation (Of word, byte just replaces popw with pop and stw with stb)
        
                current_function->instructions.push_back("\n\t; <<< STORE WORD >>> \n");

                // Get the memory information for destination
                uint64_t mem_start = ENDIAN::conditional_to_le_64(command.memory_info.start_pos);
            
                // Generate a register with the address for the destination
                std::vector<std::string> store_ins = generator.load_64_into_r0(mem_start, ("Address for [" + command.id + "]"));
                current_function->instructions.insert(current_function->instructions.end(), store_ins.begin(), store_ins.end());

                // Add the relative address of the item to the start position of the function in memory to acquire the actual destination
                current_function->instructions.push_back("\n\tldw r1 $0(ls)\t; Start of stack frame\n");
                current_function->instructions.push_back("\n\tadd r0 r0 r1\t; Item location in function memory\n");

                // Get the calculation result off the stack
                current_function->instructions.push_back("\n\t; ---- Get result ----\n");
                current_function->instructions.push_back("\tpopw r8 ls\n");

                // Store the result at the memory address
                current_function->instructions.push_back("\n\t; ---- Store result ---- \n");
                current_function->instructions.push_back("\tstw r0(gs) r8\t ; Store in memory\n");
                break;
        */
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

        /*
        
        Previous Implementations : 

            current_function->instructions.push_back("\n\t; <<< POW >>> \n");
            current_function->instructions.push_back("\n\tpopw r2 ls \t ; Calculation RHS\n\tpopw r1 ls \t ; Calculation LHS\n");

            std::string function_name;

            generator.include_builtin_math_pow(command.classification, function_name);

            current_function->instructions.push_back("\n\tcall " + function_name + " ; Call to perfom power\n\n");
            current_function->instructions.push_back("\tpushw ls r0\t; Push value to local stack for calculation\n");
            break;

        // ----- 

            current_function->instructions.push_back("\n\t; <<< MOD >>> \n");
            current_function->instructions.push_back("\n\tpopw r2 ls \t ; Calculation RHS\n\tpopw r1 ls \t ; Calculation LHS\n");

            std::string function_name;
            generator.include_builtin_math_mod(command.classification, function_name);

            current_function->instructions.push_back("\n\tcall " + function_name + " ; Call to perfom modulus\n\n");
            current_function->instructions.push_back("\tpushw ls r0\t; Push value to local stack for calculation\n");
            break;
        
        */
    };

    //
    //  Move Address
    //
    class MoveAddress : public Block
    {
    public:
        MoveAddress(CODEGEN::TYPES::MoveInstruction * ins) : Block()
        {
            code.push_back(std::string(NLT) + "; <<< MOVE ADDRESS >>> " + std::string(NLT));

            std::vector<std::string> store_ins = load_64_into_r0(ins->source, "Address of item in expression");
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            // This should be filtered by now, but just in case.
            if(ins->destination > 4294967290)
            {
                std::cerr << "CodeBlock::MoveAddress() : Given address greater than approx 2^32 - Not currently supported" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::stringstream ss;
            ss << NLT 
               << "ldw"  << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
               << "add"  << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
               << "stw $"<< ins->destination << "(gs)" << WS << "r" << REG_ADDR_RO << TAB << "; Store address in gs location" << NL;

            code.push_back(ss.str()); 
        }

        /*
        
        Previous Implementation:

            current_function->instructions.push_back("\n\t; <<< MOVE ADDRESS >>> \n");
            CODEGEN::TYPES::MoveInstruction * mins = static_cast<CODEGEN::TYPES::MoveInstruction*>(ins);
            current_function->instructions.push_back("\n\tldw r0 $0(ls)\n");
            std::vector<std::string> mov = generator.load_64_into_r0(ENDIAN::conditional_to_le_64(mins->source), "Local index of variable for move");
            current_function->instructions.insert(current_function->instructions.end(), mov.begin(), mov.end());
            current_function->instructions.push_back("\tldw r1 $0(ls) \t; Move offset\n");
            current_function->instructions.push_back("\tadd r0 r0 r1 \t ; Get absolute address for variable \n");
            current_function->instructions.push_back("\tstw $" + std::to_string(mins->destination) + "(gs) r0 \t ; Store address in gs destination\n");

        */
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

        /*
            Previous Implementation

                current_function->instructions.push_back("\n\t; <<< CALL >>> \n");

                CODEGEN::TYPES::CallInstruction * cins = static_cast<CODEGEN::TYPES::CallInstruction*>(ins);
                
                current_function->instructions.push_back("\n\tcall " + cins->function_name + "\t ; Call function\n\n");

                if(cins->expect_return_value)
                {
                    current_function->instructions.push_back("\t; Get result from call \n\n");
                    current_function->instructions.push_back("\tldw r0 $" + std::to_string(SETTINGS::GS_INDEX_RETURN_SPACE) + "(gs)\n");
                    current_function->instructions.push_back("\tpushw ls r0\t; Push return value to local stack for calculation\n");
                }
        */
    };

    //
    //  SetupPrimitive 
    //
    class SetupPrimitive : public Block
    {
    public:
        SetupPrimitive(CODEGEN::TYPES::DataClassification classification, std::string id, std::string value) : Block()
        {
            std::stringstream ss;
            switch(classification)
            {
                /*
                        Integer
                */
                case CODEGEN::TYPES::DataClassification::INTEGER: 
                {
                    // Get the numerical value
                    int64_t i_value = std::stoll(value);

                    // Conditionally little-endian the thing and convert it to an unsigned value
                    uint64_t unsigned_value = ENDIAN::conditional_to_le_64(static_cast<uint64_t>(i_value));

                    code.push_back(std::string(NLT) + "; <<< SETUP INT >>> " + std::string(NL));

                    // Generate the store for 64 bit
                    std::vector<std::string> store_ins = load_64_into_r0(unsigned_value, id);
                    code.insert(code.end(), store_ins.begin(), store_ins.end());
                    break;
                }
                /*
                        Double
                */
                case CODEGEN::TYPES::DataClassification::DOUBLE: 
                {
                    code.push_back(std::string(NLT) + "; <<< SETUP REAL >>> " + std::string(NL));

                    uint64_t unsigned_value = ENDIAN::conditional_to_le_64(
                                                UTIL::convert_double_to_uint64(std::stod(value)
                                                )
                                            );
                    
                    // Generate the store for 64 bit
                    std::vector<std::string> store_ins = load_64_into_r0(unsigned_value, id);
                    code.insert(code.end(), store_ins.begin(), store_ins.end());                       
                    break;
                }
                /*
                        Char
                */
                case CODEGEN::TYPES::DataClassification::CHAR: 
                {
                    code.push_back(std::string(NLT) + "; <<< SETUP CHAR >>> " + std::string(NL));

                    char c_val = value[1];
                    uint32_t u_val = static_cast<uint32_t>(c_val);

                    ss << NLT << "mov" << WS << "r" << REG_ADDR_RO << WS << "$" << std::to_string(u_val) << TAB << "; Move char into reg" << NL;
                    break;
                }
                default:
                    std::cerr << "Error : CodeBlock::SetupValue : Default case reached" << std::endl;
                    exit(EXIT_FAILURE);
                    break;
            }

            ss << NLT << "; ---- Move the value ----" << NLT
                << "pushw" << WS << CALC_STACK << WS << "r" << REG_ADDR_RO << TAB << "; Place on calc stack" << NL;

            code.push_back(ss.str()); 
        }
    };

}
}

#endif