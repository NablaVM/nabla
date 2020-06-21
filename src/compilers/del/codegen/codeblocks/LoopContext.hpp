#ifndef DEL_LOOP_CONTEXT_BLOCK_HPP
#define DEL_LOOP_CONTEXT_BLOCK_HPP

#include "BlockAggregator.hpp"
#include "LoadStore.hpp"
#include <iostream>

namespace DEL
{
namespace CODE
{
namespace 
{
    // This increases over the lifespan of the program. Hopefully people 
    // dont end up making 2^64-1 loops. 
    static uint64_t LOOP_CONTEXT_COUNTER = 0;
}

    //! \brief A loop context aggregator that builds instructions
    class LoopContext : public BlockAggregator
    {
    public:
        LoopContext(CODEGEN::TYPES::LoopInitiation loop_init) : loop_info(loop_init)
        {
            loop_label = "loop_context_" + std::to_string(LOOP_CONTEXT_COUNTER++);

            std::stringstream ss;
            ss << NLT 
               << "; <<< LOOP >>>" << NL << NL
               << loop_label << ":";

            instructions.push_back(ss.str());

        }

        //! \brief Export the aggregated instructions as a block
        //!        so it can be stored into another aggregator
        Block * export_as_block()
        {
            // Load end var
            {
                // Generate the code for loading the variable
                CODEGEN::TYPES::AddressValueInstruction * loader = new CODEGEN::TYPES::AddressValueInstruction(CODEGEN::TYPES::InstructionSet::LOAD, 
                    loop_info.end_var.start_pos,
                    loop_info.end_var.bytes_requested
                    );

                CODE::Load * load_ins = new CODE::Load(loader);

                // Get the generated code
                std::vector<std::string> li = load_ins->get_code();
                delete load_ins;
                delete loader;

                // Add load code
                instructions.insert(instructions.end(), li.begin(), li.end());
            }

            // Load loop var
            {
                // Generate the code for loading the variable
                CODEGEN::TYPES::AddressValueInstruction * loader = new CODEGEN::TYPES::AddressValueInstruction(CODEGEN::TYPES::InstructionSet::LOAD, 
                    loop_info.loop_var.start_pos,
                    loop_info.loop_var.bytes_requested
                    );

                CODE::Load * load_ins = new CODE::Load(loader);

                // Get the generated code
                std::vector<std::string> li = load_ins->get_code();
                delete load_ins;
                delete loader;

                // Add load code
                instructions.insert(instructions.end(), li.begin(), li.end());
            }

            std::vector<std::string> step_load;

            // Load step into r0
            if(loop_info.classification == CODEGEN::TYPES::DataClassification::INTEGER)
            {
                step_load = load_64_into_r0(std::stoull(loop_info.step), "Step value - int");
            }
            else
            {
                step_load = load_64_into_r0(UTIL::convert_double_to_uint64(std::stod(loop_info.step)), "Step value - real");
            }

            instructions.insert(instructions.end(), step_load.begin(), step_load.end());

            // Calculate and store new loop variable
            {
                std::stringstream ss; 

                if(loop_info.classification == CODEGEN::TYPES::DataClassification::INTEGER)
                {
                    ss << NLT 
                    << "popw  r1 ls"    << TAB << "; Load loop variable into r1"           << NLT 
                    << "add   r0 r0 r1" << TAB << "; Add step to loop variable into r0"    << NLT
                    << "pushw ls r0"    << TAB << "; Put the new loop var in ls"           << NLT
                    << "pushw ls r0"    << TAB << "; Store again so store_ins can take one"<< NLT;
                }
                else
                {
                    ss << NLT 
                    << "popw  r1 ls"    << TAB << "; Load loop variable into r1"           << NLT 
                    << "add.d r0 r0 r1" << TAB << "; Add step to loop variable into r0"    << NLT
                    << "pushw ls r0"    << TAB << "; Put the new loop var in ls"           << NLT
                    << "pushw ls r0"    << TAB << "; Store again so store_ins can take one"<< NLT;
                }
                instructions.push_back(ss.str());

                CODE::Store * store_ins = new CODE::Store(loop_info.loop_var.start_pos, 
                                                          loop_info.loop_var.bytes_requested,
                                                          "Loop Variable");

                std::vector<std::string> si = store_ins->get_code();
                delete store_ins;

                // Add load code
                instructions.insert(instructions.end(), si.begin(), si.end());
            }

            instructions.push_back(std::string(NLT) + "; Dealloc items alloced in loop" + std::string(NL));


            // Dealloc any new items in the loop
            while(!allocs.empty())
            {
                std::vector<std::string> addr_ins = load_64_into_r0(allocs.top().start_pos, 
                                                                    "Item start");
                instructions.insert(instructions.end(), addr_ins.begin(), addr_ins.end());

                std::stringstream ss;
                ss << NLT 
                << "ldw"  << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
                << "add"  << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
                << "ldw r0 r" << REG_ADDR_RO << "(gs)" << TAB << "; Load the DS Address from memory for dealloc" << NLT
                << "call __del__ds__free" << NL;

                instructions.push_back(ss.str());
                allocs.pop();
            }

            // Compare and conditionally jump
            {
                std::stringstream ss;
                ss << NLT
                   << "popw r0 ls" << TAB << "; Get the step variable " << NLT 
                   << "popw r1 ls" << TAB << "; Get the end variable"   << NLT 
                   << "blt r0 r1 " << loop_label << NLT;

                instructions.push_back(ss.str());
            }

            return new Export(instructions);
        }

    private:

        std::string loop_label;
        CODEGEN::TYPES::LoopInitiation loop_info;

        // Class to export the context as a block
        class Export : public Block
        {
        public:
            Export(std::vector<std::string> ins) : Block()
            {
                code.insert(code.end(), ins.begin(), ins.end());
            } 
        };
    };

}
}

#endif 