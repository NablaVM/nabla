#ifndef DEL_LOAD_STORE_BLOCKS_HPP
#define DEL_LOAD_STORE_BLOCKS_HPP

#include "Codeblock.hpp"

namespace DEL
{
namespace CODE
{
namespace
{
    static uint64_t SUCCESS_LABEL_COUNTER = 0;
}

    //
    //  Load
    //
    class Load : public Block
    {
    public:
        Load(CODEGEN::TYPES::AddressValueInstruction * ins) : Block()
        {
            std::string title_comment = "; <<< LOAD >>>";

            code.push_back(std::string(NLT) + title_comment + std::string(NL));

            // Create move instruction
            std::vector<std::string> store_ins = load_64_into_r0(ins->value, "Address of item in expression");
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            std::stringstream ss;

            ss << NL << NLT
               << "ldw" << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
               << "add" << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
               << "ldw r3 r" << REG_ADDR_RO << "(gs)" << TAB << "; Load the DS Address" << NLT
               << "pushw ls r3" << NL;

            code.push_back(ss.str()); 

            store_ins.clear();
            store_ins = load_64_into_r0(ins->bytes / SETTINGS::SYSTEM_WORD_SIZE_BYTES, "Bytes to load");
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            std::stringstream ss1;

            ss1 << NL << NLT
                << "mov r1 r0" << TAB << "; Move the words to load to r1 for call" << NLT 
                << "popw r0 ls"<< TAB << "; Pop the DS Address into r0 for call" << NL << NLT 
                << "call __del__ds__load" << NL << NLT;

            std::string load_label = "load_success_label_" + std::to_string(SUCCESS_LABEL_COUNTER++);

            ss1 << "mov r1 $0" << TAB << "; Move 0 into r1 to check for success" << NLT
                << "beq r0 r1" << WS  << load_label << NLT 
                << "; Failure to load causes an EXIT" << NLT 
                << "exit" << NL << NL
                << load_label << ":" << NL << NLT
                << "; Move loaded words off of the GS and into LS" << NL;

            for(int i = 0; i < ins->bytes / SETTINGS::SYSTEM_WORD_SIZE_BYTES; i++)
            {
                ss1 << NLT
                    << "popw r0 gs" << NLT
                    << "pushw ls r0" << NL;
            }

            code.push_back(ss1.str()); 
        }
    };

    //
    //  Store
    //
    class Store : public Block
    {
    public:
        Store(uint64_t mem_start, uint64_t byte_len, std::string id) : Block()
        {
            std::string title_comment = "; <<< STORE >>>";
            std::string address_comment = "Address for [ " + id + " ]";

            code.push_back(std::string(NLT) + title_comment + std::string(NL));

            // Create move instruction
            std::vector<std::string> store_ins = load_64_into_r0(mem_start, address_comment);
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            std::stringstream ss;
            ss << NLT 
               << "ldw"  << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
               << "add"  << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
               << "ldw r0 r" << REG_ADDR_RO << "(gs)" << TAB << "; Load the DS Address from memory for [" << id << "] into r0 for call" << NLT
               << "size r1 gs" << TAB << "; Get current size of GS into r1 for call" << NL << NLT
               << "; Get words from local stack an put on gs for transit" << NL;

            for(int i = 0; i < byte_len / SETTINGS::SYSTEM_WORD_SIZE_BYTES; i++)
            {
                ss << NLT
                   << "popw r5" << WS << CALC_STACK << TAB << "; Get word from LS" << NLT 
                   << "pushw gs r5" << TAB << "; Push to GS";
            }

            std::string store_label = "store_success_label_" + std::to_string(SUCCESS_LABEL_COUNTER++);

            ss << NL << NLT 
               << "size r2 gs" << TAB << "; Get new size of GS into r2 for call" << NL << NLT
               << "call __del__ds__store" << NL << NLT
               << "mov r1 $0" << TAB << "; Move 0 into r1 to check for success" << NLT
               << "beq r0 r1" << WS  << store_label << NLT 
               << "; Failure to store causes an EXIT" << NLT 
               << "exit" << NL << NL
               << store_label << ":" << NL;

            code.push_back(ss.str()); 

        }
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
    };

}
}

#endif