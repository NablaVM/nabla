#ifndef DEL_ALLOC_BLOCKS_HPP
#define DEL_ALLOC_BLOCKS_HPP

#include "Codeblock.hpp"

namespace DEL
{
namespace CODE
{
    //
    //  DS Allocate
    //
    class DSAllocate : public Block
    {
    public:
        DSAllocate(DEL::CODEGEN::TYPES::DSAllocInstruction * ins, uint64_t mem_start) : Block()
        {
            std::string title_comment = "; <<< DS ALLOC >>>";
            code.push_back(std::string(NL) + std::string(NLT) + title_comment + std::string(NL));

            std::vector<std::string> store_ins = load_64_into_r0(ins->bytes_to_alloc, "Bytes to allocate");
            code.insert(code.end(), store_ins.begin(), store_ins.end());

            std::stringstream ss;

            ss << NLT
               << "call __del__ds__alloc"
               << NLT
               << "pushw ls r0"
               << NL;

            code.push_back(ss.str());

            std::vector<std::string> store_ins1 = load_64_into_r0(mem_start, "Load memory location");
            code.insert(code.end(), store_ins1.begin(), store_ins1.end());

            std::stringstream ss1;
            ss1 << NLT 
               << "ldw"  << WS << "r" << REG_ADDR_SP << WS << "$0(ls)" << TAB << "; Load SP into local stack" << NLT 
               << "add"  << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_RO << WS << "r" << REG_ADDR_SP << TAB << "; Item location in function mem" << NL << NLT
               << "; ---- Get DS Address ---- " << NL << NLT
               << "popw" << WS << "r" << REG_ARITH_LHS << WS << CALC_STACK << NL << NLT
               << "; ---- Store DS Address ---- " << NL << NLT
               << "stw" << WS << "r" << REG_ADDR_RO << "(gs)" << WS << "r" << REG_ARITH_LHS << TAB << "; Store address in memory" << NL;

            code.push_back(ss1.str());
        }
    };
}
}

#endif 