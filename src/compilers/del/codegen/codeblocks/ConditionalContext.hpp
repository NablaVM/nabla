/*
    The conditional context builds chains of if statements together. When an elseif or else is caught,
    it comes down to this extend_context. Extend context adds the load / check code required to
    access the relevant statements corresponding to the if/elif/else locations. It does this by 
    populating a "branches" vector with the load code, and the typical "instructions" vector in tandem,
    then, when exported, the result will be the branches vector contents followed by the instructions. 
*/

#ifndef DEL_CONDITIONAL_CONTEXT_BLOCK_HPP
#define DEL_CONDITIONAL_CONTEXT_BLOCK_HPP

#include "BlockAggregator.hpp"
#include "LoadStore.hpp"

#include <stack>
#include <iostream>

namespace DEL
{
namespace CODE
{
namespace 
{
    // This increases over the lifespan of the program. Hopefully people 
    // dont end up making 2^64-1 if/elif/else chains. 
    static uint64_t CONTEXTUAL_COUNTER = 0;
}

    //! \brief A conditional context aggregator that builds instructions
    //!        for a particular context that is within a function context
    class ConditionalContext : public BlockAggregator
    {
    public:

        //! \brief Create the conditional context
        ConditionalContext(CODEGEN::TYPES::ConditionalInitiation init)
        {
            // Need to have address of artificial variable
            // for enterying context given to this constructor
            bottom_label = "conditional_context_bottom_" + std::to_string(CONTEXTUAL_COUNTER++);

            load_item_and_labels(init);
        }

        //! \brief Extend the context to catch a different condition if the previous failed
        void extend_context(CODEGEN::TYPES::ConditionalInitiation init)
        {
            // Add jmp to skip over extension if the previous statement was accessed
            std::stringstream ss;
            ss << NL     << NLT << "; Jump to skip next segment if current is accessed " << NL << NLT 
               << "jmp " << bottom_label << NL;

            // Free currently allocated stuff
            free_context_variables();
            instructions.insert(instructions.end(), frees.begin(), frees.end());
            frees.clear();

            instructions.push_back(ss.str());

            load_item_and_labels(init);
        }

        //! \brief Export the aggregated instructions as a block
        //!        so it can be stored into another aggregator
        Block * export_as_block()
        {
            // Add jmp to skip over everything if nothing was true
            std::stringstream ss;
            ss << NL     << NLT << "; Jump if nothing is to be executed " << NL << NLT 
               << "jmp " << bottom_label << NL;

            branches.push_back(ss.str());
            
            // Free currently allocated stuff
            free_context_variables();
            instructions.insert(instructions.end(), frees.begin(), frees.end());
            frees.clear();

            // Add bottom label
            std::stringstream ss1;
            ss1 << NL << NL
               << bottom_label << ":" << NL;
            instructions.push_back(ss1.str());

            // Result vector
            std::vector<std::string> result;

            // Combine the instructions and the pre-fixed branch statements
            result.insert(result.end(), branches.begin(),     branches.end());
            result.insert(result.end(), instructions.begin(), instructions.end());

            return new Export(result);
        }

    private:

        std::vector<std::string> frees;
        std::vector<std::string> branches;
        std::string bottom_label;


        void load_item_and_labels(CODEGEN::TYPES::ConditionalInitiation init)
        {
            std::stringstream ss;

            ss << NLT
               << "; <<< CONDITIONAL CONTEXT >>>"
               << NL;

            branches.push_back(ss.str());
        
            // Generate the code for loading the conditional variable
            CODEGEN::TYPES::AddressValueInstruction * loader = new CODEGEN::TYPES::AddressValueInstruction(CODEGEN::TYPES::InstructionSet::LOAD, 
                init.mem_info.start_pos,
                init.mem_info.bytes_requested
                );

            CODE::Load * load_ins = new CODE::Load(loader);

            // Get the generated code
            std::vector<std::string> li = load_ins->get_code();
            delete load_ins;
            delete loader;

            // Add generated code to 
            branches.insert(branches.end(),li.begin(), li.end());

            std::stringstream ss1;

            // Create code for pulling loaded value and comparing against 1 to 
            // see if we should plop into conditional block
            std::string label = "if_label_" + std::to_string(CONTEXTUAL_COUNTER++);

            ss1 << NLT 
               << "popw r0 ls"  << TAB   << "; Load value of check into r0" << NLT
               << "mov  r1 $1"  << TAB   << "; Comparison value" << NLT
               << "beq  r0 r1 " << label << TAB << "; Branch if true" <<  NL;

            branches.push_back(ss1.str());
          
            // Add the label 
            instructions.push_back(std::string(NL) + label + ":" + std::string(NL));
        }

        //  Free variables created within the current context
        //
        void free_context_variables()
        {
            frees.push_back(std::string(NLT) + "; Dealloc items alloced in loop" + std::string(NL));

            std::stringstream ss1;
            ss1 << NLT << "; <<< CONTEXTUAL FREE >>>" << NLT 
                << ";--------------------------------------" << NL;

            frees.push_back(ss1.str());
        
            // Dealloc any new items in the loop
            while(!allocs.empty())
            {
                std::vector<std::string> addr_ins = load_64_into_r0(allocs.top().start_pos, 
                                                                    "Item start");
                frees.insert(frees.end(), addr_ins.begin(), addr_ins.end());

                std::stringstream ss;
                ss << NLT 
                << "ldw"  << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
                << "add"  << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
                << "ldw r0 r" << REG_ADDR_RO << "(gs)" << TAB << "; Load the DS Address from memory for dealloc" << NLT
                << "call __del__ds__free" << NL;

                frees.push_back(ss.str());
                allocs.pop();
            }
        }

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