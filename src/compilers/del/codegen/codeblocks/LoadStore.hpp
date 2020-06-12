#ifndef DEL_LOAD_STORE_BLOCKS_HPP
#define DEL_LOAD_STORE_BLOCKS_HPP

#include "Codeblock.hpp"

namespace DEL
{
namespace CODE
{
    //
    //  Load
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
    //  Store
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

}
}

#endif