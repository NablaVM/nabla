#ifndef DEL_CONDITIONAL_CONTEXT_BLOCK_HPP
#define DEL_CONDITIONAL_CONTEXT_BLOCK_HPP

#include "BlockAggregator.hpp"

namespace DEL
{
namespace CODE
{
    //! \brief A conditional context aggregator that builds instructions
    //!        for a particular context that is within a function context
    class ConditionalContext : public BlockAggregator
    {
    public:

        //! \brief Create the conditional context
        ConditionalContext()
        {
            // Need to have address of artificial variable
            // for enterying context given to this constructor
            
          
        }

        //! \brief Export the aggregated instructions as a block
        //!        so it can be stored into another aggregator
        Block * export_as_block()
        {
            return new Export(this->instructions);
        }

    private:

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