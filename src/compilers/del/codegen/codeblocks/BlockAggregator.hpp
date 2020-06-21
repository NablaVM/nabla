#ifndef DEL_BLOCK_AGGREGATOR_HPP
#define DEL_BLOCK_AGGREGATOR_HPP

#include "Codeblock.hpp"
#include "Memory.hpp"
#include <stack>
namespace DEL
{
namespace CODE
{
    //
    //  A consumer of blocks
    //
    class BlockAggregator
    {
    public:

        //  Copies block data to local vector and deletes the block
        //
        void add_block(CODE::Block * block)
        {
            std::vector<std::string> code = block->get_code();
            instructions.insert(instructions.end(), code.begin(), code.end());
            delete block;
        }

        // Add memory allocation so aggregator can clean 
        //
        void add_memory_alloc(Memory::MemAlloc mem_info)
        {
            allocs.push(mem_info);
        }

    protected:
        std::vector<std::string> instructions; 
        std::stack<Memory::MemAlloc > allocs;
    };
}
}


#endif
