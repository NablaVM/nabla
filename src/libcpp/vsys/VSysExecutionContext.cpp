#include "VSysExecutionContext.hpp"
#include "VSysMachine.hpp"

namespace NABLA
{
namespace VSYS
{
    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    ExecutionContext::ExecutionContext(Machine &owner,
                                       uint64_t startAddress, 
                                       Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory, 
                                       std::vector< std::vector<uint64_t> > &functions) :
                                                                                         owner(owner),
                                                                                         contextCompleted(false),
                                                                                         global_memory(global_memory),
                                                                                         functions(functions),
                                                                                         currentInstructionBlock(startAddress)

    {
        
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    ExecutionContext::~ExecutionContext()
    {

    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    bool ExecutionContext::isContextComplete() const
    {
        return contextCompleted;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    ExecutionReturns ExecutionContext::stepExecution(uint64_t steps)
    {
        return ExecutionReturns::OKAY;
    }
}
}