#ifndef DEL_BLOCK_HPP
#define DEL_BLOCK_HPP

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
}
}



#endif