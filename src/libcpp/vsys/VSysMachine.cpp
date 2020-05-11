#include "VSysMachine.hpp"

namespace NABLA
{
namespace VSYS
{
    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
    
    Machine::Machine() : running(false), inErrorState(false), entryAddress(0)
    {

    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
    

    Machine::~Machine()
    {

    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
    
    ExecutionReturns Machine::step(uint64_t steps)
    {
        if(inErrorState)
        {
            return ExecutionReturns::EXECUTION_ERROR;
        }

        if(!running)
        {
            executionContexts.clear();
            running = true;

            if(!newExecutionContext(entryAddress))
            {
                inErrorState = true;
                return ExecutionReturns::FAILED_TO_SPAWN_EXECUTION_CONTEXT;
            }
        }

        for(uint64_t idx = 0; idx < executionContexts.size(); idx++)
        {
            /*
                    This needs to be updated to allow detailed error reporting. 

                    - Execution context ID, the error etc
            */
            
            switch(executionContexts[idx].stepExecution(steps))
            {
                case ExecutionReturns::OKAY:                              break;
                case ExecutionReturns::ALL_EXECUTION_COMPLETE:            break;                 
                case ExecutionReturns::INSTRUCTION_NOT_FOUND:             return ExecutionReturns::INSTRUCTION_NOT_FOUND;
                case ExecutionReturns::UNKNOWN_INSTRUCTION:               return ExecutionReturns::UNKNOWN_INSTRUCTION;
                case ExecutionReturns::FAILED_TO_SPAWN_EXECUTION_CONTEXT: return ExecutionReturns::FAILED_TO_SPAWN_EXECUTION_CONTEXT;
                case ExecutionReturns::EXECUTION_ERROR:                   return ExecutionReturns::EXECUTION_ERROR;
            }
        }

        return ExecutionReturns::OKAY;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
    

    bool Machine::newExecutionContext(uint64_t address)
    {
        if(address > functions.size()) { return false; }

        executionContexts.push_back(
            ExecutionContext(*this, address, global_memory, functions)
        );

        return true;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
    
    void Machine::executionContextGarbageCollection()
    {
        std::vector<ExecutionContext> tmp;

        for(auto & ec : executionContexts)
        {
            if(!ec.isContextComplete())
            {
                tmp.push_back(ec);
            }
        }

        executionContexts.clear();

        executionContexts.swap(tmp);
    }

}
}