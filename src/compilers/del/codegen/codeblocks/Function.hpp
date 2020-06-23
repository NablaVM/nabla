#ifndef DEL_FUNCTION_BLOCK_HPP
#define DEL_FUNCTION_BLOCK_HPP

#include "BlockAggregator.hpp"

namespace DEL
{
namespace CODE
{
namespace
{
    static int LABEL_COUNTER = 0;
}
    //
    //  A function
    //
    //      The function object collects blocks during code generation, and upon calling building_complete
    //      will add the instructions for setting up the stack frame and loading parameters. This ASM is not
    //      configurable with the above code as it is "standalone" in the sense that no actions by the user can
    //      change make any changes to it no matter what instruction is given.
    //
    class Function : public BlockAggregator
    {
    public:
        Function(std::string name, std::vector<CODEGEN::TYPES::ParamInfo> params) : name(name), params(params), bytes_required(0)
        {
            // Allocate a space for each parameter
            //
            for(auto & p : params)
            {
                uint64_t bytes_for_param = p.end_pos - p.start_pos;

                bytes_required += bytes_for_param;
            }
        }

        // ----------------------------------------
        //
        // ----------------------------------------

        void add_required_bytes(uint64_t num_bytes)
        {
            bytes_required += num_bytes;
        }

        // ----------------------------------------
        //
        // ----------------------------------------

        std::vector<std::string> building_complete()
        {
            // Putting this limit in place while we get things working
            if(bytes_required >= 2147483647)
            {
                std::cerr << "CodeBlock::Function >>> Function size is currently limited to bytes represented by an int32_t (2147483647 bytes)" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::vector<std::string> lines;

            std::stringstream ss;
            ss << NL << NL << "<" << name << ":" << NL << NLT
               << "ldw r8 $0(gs)" << TAB << "; Load the current stack pointer" << NLT 
               << "pushw ls r8"   << TAB << "; Store the local stack pointer in function memory" << NLT 
               << "mov r9 $"      << ENDIAN::conditional_to_le_64(bytes_required/8) << TAB << "; Words required for function (" << name << ")" << NLT
               << "pushw ls r9"   << TAB << "; Store size in local funtion memory" << NL << NLT
               << "; Expand GS to store current function" << NL << NLT
               << "mov r1 $0" << NLT
               << "mov r2 $0" << NL << NL
               << "function_alloc_gs:" << NLT
               << "add r1 r1 $1" << NLT
               << "pushw gs r2" << NLT
               << "blt r1 r9 function_alloc_gs" << NL << NLT
               << "mov r9 $" << ENDIAN::conditional_to_le_64(bytes_required) << TAB << "; Bytes reqired for function (" << name << ")" << NLT
               << "add r8 r8 r9"  << TAB << "; Add function size to the stack pointer" << NLT 
               << "stw $0(gs) r8" << TAB << "; Increase the stack pointer" << NL;

            lines.push_back(ss.str());

            for(auto & p : params)
            {
                std::stringstream ssp;

                std::vector<std::string> store_ins = load_64_into_r0(ENDIAN::conditional_to_le_64(p.start_pos), "Load relative parameter destination");
                lines.insert(lines.end(), store_ins.begin(), store_ins.end());

                ssp << NLT 
                    << "ldw r1 $0(ls)" << TAB << "; Load stack pointer for this function" << NLT 
                    << "add r0 r0 r1"  << TAB << "; Get absolute address for parameter copying" << NLT 
                    << "ldw r1 $" << p.param_gs_index << "(gs)" << TAB << "; Load parameters address" << NLT;

                
                if(p.start_pos == p.end_pos - 1)
                {
                    ssp << "ldb r1 r1(gs)" << TAB << "; Load parameter value into r1" << NLT
                        << "stb r0(gs) r1" << TAB << "; Store in local frame" << NL;
                }
                else
                {
                    ssp << "ldw r1 r1(gs)" << TAB << "; Load parameter value into r1" << NLT
                        << "stw r0(gs) r1" << TAB << "; Store in local frame" << NL;
                }
                lines.push_back(ssp.str());
            }

            // Add user given instruction block data
            lines.insert(lines.end(), instructions.begin(), instructions.end());

            // Add function term
            lines.push_back(std::string(NL) + ">" + std::string(NL));

            instructions.clear();
            return lines;
        }

        // ----------------------------------------
        //
        // ----------------------------------------

        void build_return(bool return_item = true)
        {
            std::string dealloc_label = "function_dealloc_gs_" + std::to_string(LABEL_COUNTER++);
            /*
                The loop code for shrinking GS is not set to be configurable on purpose
            */
            std::stringstream ss;
            ss << NLT << "; <<< RETURN >>>" << NL
               << NLT << "ldw" << WS << "r" << REG_ARITH_LHS << WS << "$0(" << CALC_STACK << ")" << TAB << "; Initial stack pointer"
               << NLT << "ldw" << WS << "r" << REG_ARITH_RHS << WS << "$8(" << CALC_STACK << ")" << TAB << "; Function size (words)"
               << NLT << "stw" << WS << "$0(gs)" << WS << "r" << REG_ARITH_LHS << TAB << "; Reset stack pointer" << NL
               << NLT << "; Shrink GS to clean up current function" << NL 
               << NLT << "mov r1 $0" << NL
               << NL  << dealloc_label << ":"
               << NLT << "add r1 r1 $1"
               << NLT << "popw r0 gs"
               << NL
               << NLT << "; Free memory from DS Device - Developer note : This might cause some return issues later on"
               << NLT << "call __del__ds__free"
               << NLT << "blt r1 r9" << WS << dealloc_label << NL
               << NL;

            if(return_item)
            {
                ss << NLT
                   << "; Get result for return"
                   << NLT << "popw" << WS << "r" << REG_ADDR_RO << WS << CALC_STACK
                   << NLT << "stw $" << SETTINGS::GS_INDEX_RETURN_SPACE << "(gs)" << WS << "r" << REG_ADDR_RO << NL << NL;
            }

            ss << TAB << "ret" << NL;

            instructions.push_back(ss.str());
        }

    private:
        std::string name;                                   //! The name of the function
        std::vector<CODEGEN::TYPES::ParamInfo> params;      //! The parameter information given to the function
        uint64_t bytes_required;                            //! How many bytes of stack space the function will take up
    };
}
}


#endif