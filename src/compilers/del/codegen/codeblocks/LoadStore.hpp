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
        Load(CODEGEN::TYPES::AddressValueInstruction * ins) : Block()
        {
            std::string title_comment = "; <<< LOAD >>>";

            code.push_back(std::string(NLT) + title_comment + std::string(NL));

            // Create move instruction
            std::vector<std::string> store_ins = load_64_into_r0(ins->value, "Address of item in expression");
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            std::stringstream ss;

           ss << NL << NLT
              << "; Get GS size and store for use. Generate space in GS for load" << NLT 
              << "size r5 gs"  << NLT 
              << "pushw ls r5" << NLT
              << "mov r9 $0"   << NL;

            for(int i = 0; i < ins->bytes / SETTINGS::SYSTEM_WORD_SIZE_BYTES; i++)
            {
                ss << NLT
                   << "pushw gs r9";
            }

            ss << NLT
               << "; Get new gs size" << NLT
               << "size r5 gs"        << NLT
               << "pushw ls r5"       << NL << NLT 
               << "ldw" << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
               << "add" << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
               << "ldw r3 r" << REG_ADDR_RO << "(gs)" << TAB << "; Load the DS Address" << NLT
               << "mov r11 r3"  << TAB << "; Move address to target register r11" << NLT
               << "popw r3 ls " << TAB << "; End idx" << NLT 
               << "popw r4 ls " << TAB << "; Start idx" << NLT
               << "lsh r4 r4 $32 ; Shift start index prepping for OR" << NLT
               << "or r12 r3 r4" << NLT
               << "lsh r0 $13 $56 ; Move DS id into position" << NLT 
               << "lsh r1 $20 $48" << NLT
               << "or r10 r0 r1" << NL;

            ss << NLT << "; Get data from gs and store in ls" << NL;

            for(int i = 0; i < ins->bytes / SETTINGS::SYSTEM_WORD_SIZE_BYTES; i++)
            {
                ss << NLT
                   << "popw r0 gs" << NLT
                   << "pushw ls r0" << NL;
            }

            code.push_back(ss.str()); 
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
               << "ldw r3 r" << REG_ADDR_RO << "(gs)" << TAB << "; Load the DS Address from memory for [" << id << "]" << NLT
               << "size r4 gs" << TAB << "; Get current size of GS" << NL << NLT
               << "; Get words from local stack an put on gs for transit" << NL;

            for(int i = 0; i < byte_len / SETTINGS::SYSTEM_WORD_SIZE_BYTES; i++)
            {
                ss << NLT
                   << "popw r5" << WS << CALC_STACK << TAB << "; Get word from LS" << NLT 
                   << "pushw gs r5" << TAB << "; Push to GS";
            }

            // r3 Has address for DS
            // r4 Has size of GS before pushs
            // Data is in GS from r4 -> current size gs

            ss << NLT 
               << "mov r11 r3    " << TAB << "; Move DS address of [" << id << "] into target register r11" << NLT
               << "size r3 gs    " << TAB << "; Get the new size of GS after pushing data to it" << NLT 
               << "lsh r0 r4 $32 " << TAB << "; Left shift start address " << NLT
               << "or r12 r0 r3  " << TAB << "; Or start and end GS locations for command into target register r12"  << NLT
               << "lsh r0 $13 $56" << TAB << "; Load DS ID " << NLT 
               << "lsh r1 $10 $48" << TAB << "; Mark sub-id for 'store' command" << NLT 
               << "or r10 r1 r0  " << TAB << "; Or command into trigger register" << NL << NLT
               << "; Clean up the GS" << NL;

            for(int i = 0; i < byte_len / SETTINGS::SYSTEM_WORD_SIZE_BYTES; i++)
            {
                ss << NLT
                   << "popw r5 gs" << NLT;
            }
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