#ifndef DEL_WHILE_LOOP_CONTEXT_BLOCK_HPP
#define DEL_WHILE_LOOP_CONTEXT_BLOCK_HPP

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
    static uint64_t WHILE_LOOP_CONTEXT = 0;
}

    //! \brief A loop context aggregator that builds instructions
    class WhileLoopContext : public BlockAggregator
    {
    public:
        WhileLoopContext(CODEGEN::TYPES::WhileInitiation * loop_init) : loop_info(loop_init)
        {
            loop_label = "while_loop_context_" + std::to_string(WHILE_LOOP_CONTEXT);
            end_of_loop_label = "while_loop_end_" + std::to_string(WHILE_LOOP_CONTEXT++);

            // Add top loop label
            {
                std::stringstream ss;
                ss << NLT 
                << "; <<< WHILE LOOP >>>" << NL << NL
                << loop_label << ":";
                instructions.push_back(ss.str());
            }

            // Load conditional variable 
            {
                CODEGEN::TYPES::AddressValueInstruction * loader = new CODEGEN::TYPES::AddressValueInstruction(CODEGEN::TYPES::InstructionSet::LOAD, 
                    loop_info->condition.start_pos,
                    loop_info->condition.bytes_requested
                    );

                CODE::Load * load_ins = new CODE::Load(loader);

                // Get the generated code
                std::vector<std::string> li = load_ins->get_code();
                delete load_ins;
                delete loader;

                instructions.insert(instructions.end(), li.begin(), li.end());
            }

            // Check if we need to jump to bottom
            {
                std::stringstream ss;

                if(loop_info->classification == CODEGEN::TYPES::DataClassification::INTEGER)
                {
                    ss << NLT 
                        << "popw r0 ls"  << TAB   << "; Load value of check into r0" << NLT
                        << "mov  r1 $1"  << TAB   << "; Comparison value" << NLT
                        << "bne  r0 r1 " << end_of_loop_label << TAB << "; Branch if true" <<  NL;
                }
                else
                {
                    ss << NLT 
                        << "popw  r0 ls"  << TAB   << "; Load value of check into r0" << NLT
                        << "mov   r1 $1"  << TAB   << "; Comparison value" << NLT
                        << "bne.d r0 r1 " << end_of_loop_label << TAB << "; Branch if true" <<  NL;
                }
                instructions.push_back(ss.str());
            }
        }

        //! \brief Export the aggregated instructions as a block
        //!        so it can be stored into another aggregator
        Block * export_as_block()
        {
            // Add memory cleanup
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

            // Add jump to top of loop and bottom label to escape loop
            {
                std::stringstream ss;
                ss << NLT
                   << "jmp" << WS << loop_label << TAB << "; Jump to top of loop" << NL << NL
                   << end_of_loop_label << ":" << NL;
                instructions.push_back(ss.str());
            }
            return new Export(instructions);
        }

    private:

        std::string loop_label;
        std::string end_of_loop_label;
        CODEGEN::TYPES::WhileInitiation * loop_info;

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