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