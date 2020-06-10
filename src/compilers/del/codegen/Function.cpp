#include "Function.hpp"
#include "Generator.hpp"
#include <libnabla/endian.hpp>
#include <iostream>
#include "SystemSettings.hpp"
namespace DEL
{
namespace CODEGEN
{

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Function::Function(std::string name, std::vector<CODEGEN::TYPES::ParamInfo> params) : 
                                                                                                          name(name), 
                                                                                                          params(params), 
                                                                                                          bytes_required(0)
    {
        // Allocate a space for each parameter
        //
        for(auto & p : params)
        {
            uint64_t bytes_for_param = p.end_pos - p.start_pos;

            bytes_required += bytes_for_param;
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Function::~Function()
    {
        instructions.clear();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::vector<std::string> Function::building_complete()
    {
        std::vector<std::string> lines;

        // Putting this limit in place while we get things working
        if(bytes_required >= 4294967290)
        {
            std::cerr << "Codegen::Function >>> Function size is currently limited to ~ 2^32 bytes" << std::endl;
            exit(EXIT_FAILURE);
        }

        lines.push_back("<" + name + ":\n\n");

        lines.push_back("\tldw r8 $0(gs)\t ; Load the current stack offset\n");
        lines.push_back("\tpushw ls r8 \t ; Store it in the local stack\n");

        lines.push_back("\tmov r9 $" + std::to_string(ENDIAN::conditional_to_le_64(bytes_required/8)) + "\t ; Words required for function (" + name + ")\n" );
        lines.push_back("\tpushw ls r9 \t ; Store it in the local stack\n\n");

        lines.push_back("\n\t; Expand GS to store the current function instance\n");
        lines.push_back("\n\tmov r1 $0\n");
        lines.push_back("\tmov r2 $0\n");
        lines.push_back("\nfunction_alloc_gs:\n");
        lines.push_back("\n\tadd r1 r1 $1\n");
        lines.push_back("\tpushw gs r2\n");
        lines.push_back("\n\tblt r1 r9 function_alloc_gs\n"); // Keep pushing words until we've taken on the stack frame

        lines.push_back("\tmov r9 $" + std::to_string(ENDIAN::conditional_to_le_64(bytes_required)) + "\t ; Bytes required for function (" + name + ")\n" );
        lines.push_back("\n\tadd r8 r8 r9 \t; Add our size to the gs offset");
        lines.push_back("\n\tstw $0(gs) r8 \t; Increates the stack offset\n");

        Generator gen;
        for(auto & p : params)
        {
            // Load our relative address for parameter destination
            std::vector<std::string> load_rel_addr = gen.load_64_into_r0(ENDIAN::conditional_to_le_64(p.start_pos), "Load relative parameter destination");
           
            lines.insert(lines.end(), load_rel_addr.begin(), load_rel_addr.end());

            // Resulting address in r0
            lines.push_back("\tldw r1 $0(ls)\t ; Load local stack offset \n");
            lines.push_back("\tadd r0 r0 r1 \t ; Get absolute address for parameter copying \n");
            
            lines.push_back("\tldw r1 $" + std::to_string(p.param_gs_index) + "(gs) \t; Load the params current address to r1\n");

            if(p.start_pos == p.end_pos - 1)
            {
               lines.push_back("\tldb r1 r1(gs) \t ; Load the params value into r1\n");
               lines.push_back("\tstb r0(gs) r1 \t ; Store the param into the local frame stack\n");
            }
            else
            {
               lines.push_back("\tldw r1 r1(gs) \t ; Load the params value into r1\n");
               lines.push_back("\tstw r0(gs) r1 \t ; Store the param into the local frame stack\n");
            }
        }
        
        lines.insert(lines.end(), instructions.begin(), instructions.end());

        lines.push_back("\n>\n");
        return lines;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Function::build_return(bool return_item)
    {
        // The expression of the return should be on the local stack now, so all we have to do is pop it off into r0
        instructions.push_back("\n\t; <<< RETURN >>> \n");

        instructions.push_back("\n\tldw r8 $0(ls) \t; Initial stack offset");
        instructions.push_back("\n\tldw r9 $8(ls) \t; Our size (in words) ");
        instructions.push_back("\n\tstw $0(gs) r8 \t; Reset stack offset\n");

        instructions.push_back("\n\t; Shrink GS to clean up the current function instance\n");
        instructions.push_back("\n\tmov r1 $0\n");
        instructions.push_back("\nfunction_dealloc_gs:\n");
        instructions.push_back("\n\tadd r1 r1 $1\n");
        instructions.push_back("\tpopw r7 gs\n");
        instructions.push_back("\n\tblt r1 r9 function_dealloc_gs\n");

        if(return_item)
        {
            instructions.push_back("\n\t; Get result for return \n");
            instructions.push_back("\tpopw r0 ls\n");
            instructions.push_back("\tstw $" + std::to_string(SETTINGS::GS_INDEX_RETURN_SPACE) + "(gs) r0\n");
        }
        
        instructions.push_back("\tret\n");
    }
}
}