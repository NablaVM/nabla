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
               << "lsh r0  r0  $16 \t; Move allocation request into place" << NLT 
               << "lsh r1  $13 $56 \t; Move DS Device ID into place"       << NLT
               << "or  r10 r0  r1  \t; Or together to create command"      << NLT
               << "pushw ls r12    \t; Store resulting address on ls"      << NL;

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