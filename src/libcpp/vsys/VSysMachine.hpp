#ifndef NABLA_VSYS_MACHINE_HPP
#define NABLA_VSYS_MACHINE_HPP

#include "VSysSettings.hpp"
#include "VSysExecutionContext.hpp"
#include "VSysExecutionReturns.hpp"
#include "VSysMemory.hpp"
#include "VSysInstructions.hpp"

#include <stdint.h>
#include <vector>

namespace NABLA
{
namespace VSYS
{
    class Machine
    {
    public:

        Machine();
        ~Machine();

        // Step each execution context by a specific number of steps
        ExecutionReturns step(uint64_t stepsPerContext);

        // Remove completed execution contexts
        void executionContextGarbageCollection();
    
    protected:

        bool running;
        bool inErrorState;

        // Address of first function to execute
        uint64_t entryAddress;

        uint64_t numberOfFunctions;

        // Global memory
        Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> global_memory;

        // Function's instructions
        std::vector< std::vector<uint64_t> > functions;

        // Each 'thread' of execution
        std::vector<ExecutionContext> executionContexts;

    private:

        // To allow execution contexts to spawn other contexts
        friend class ExecutionContext;

        // Create a new execution context (called at start of machine by machine, and triggered
        // by a currently running execution context)
        bool newExecutionContext(uint64_t address);
    };
} 
}


#endif