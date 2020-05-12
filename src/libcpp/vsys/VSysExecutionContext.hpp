#ifndef NABLA_VSYS_EXECUTION_CONTEXT_HPP
#define NABLA_VSYS_EXECUTION_CONTEXT_HPP

#include "VSysExecutionReturns.hpp"
#include "VSysMemory.hpp"
#include <stdint.h>
#include <vector>
#include <stack>

namespace NABLA
{
namespace VSYS
{
    // Fwd for machine
    class Machine;

    // Runs until the function that it is started with is completed
    class ExecutionContext
    {
    public:
        ExecutionContext(Machine &owner, uint64_t startAddress, Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory, std::vector< std::vector<uint64_t> > &functions);

        ~ExecutionContext();

        ExecutionReturns stepExecution(uint64_t steps); 

        bool isContextComplete() const;

    private:

        Machine &owner;

        bool contextCompleted;

        // Global system memory
        Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory;

        // Reference to defined functions and their instructions
        std::vector< std::vector<uint64_t> > & functions;

        // A wrapper for function instructions for information required within an
        // individual execution context
        struct InstructionBlock
        {
            uint64_t instruction_pointer;
            std::vector<uint64_t> * instructions;
            Memory<NABLA_VSYS_SETTINGS_LOCAL_MEMORY_BYTES> function_memory;
        };

        // Functions wrapped with context info for execution within the current
        // instance of ExecutionContext
        std::vector<InstructionBlock> contextFunctions;

        // The current function pointer
        uint64_t currentInstructionBlock;

        // Call stack for the current execution context
        std::stack<uint64_t> callStack;

        // Registers that can be used specific to this context
        int64_t registers[16];

        bool switchingFunction;

        void get_arith_lhs_rhs(uint8_t id, uint64_t ins, int64_t * lhs, int64_t * rhs);

        bool attempt_return();
    };
} 
}


#endif