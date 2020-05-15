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

    //! \brief An object used for the execution of byte code. The context represents a a sort of 'thread' that has its own state and variable information.
    //!        A context will step execution until it attempts to return with an empty call stack, or an 'exit' is found.
    class ExecutionContext
    {
    public:

        //! \brief Create an execution context
        //! \param owner The machine object that the context belongs to
        //! \param startAddress The function address that the context will start its execution at. 
        //! \param globalMemory The memory structure for the the machine that owns the context. This is shared with all other operational contexts
        //! \param functions The bytecode instructions that can be executed by this context
        //! \post  The context will be able to be stepped for execution
        //! \note  A context should only be used inside of a VM structure, but if one is careful, they could leverage a context as a stand-alone vm. 
        ExecutionContext(Machine &owner, uint64_t startAddress, Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory, std::vector< std::vector<uint64_t> > &functions);

        //! \brief Destruct a context
        ~ExecutionContext();

        //! \brief Step the execution of this context by 'steps' amount
        //! \param steps The number of individual instructions the context should attempt to execute
        //! \returns An ExecutionReturns enumeration that will detail the resulting executions
        ExecutionReturns stepExecution(uint64_t steps); 

        //! \brief Check if the context has completed all available execution
        //! \retval True if the context is complete, False otherwise
        bool isContextComplete() const;

        //! \brief The registers containing data for the sake of execution within this context
        //!        These are made public for testing and poking / prodding. Realistically, this _should_ be private, 
        //!        but for the sake of development and testing we are leaving it public. It doesn't really matter.
        int64_t registers[16];
        
    protected:

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

    private:

        // Owner of this context
        Machine &owner;

        // Context completed flag
        bool contextCompleted;

        // Global system memory
        Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory;

        // Reference to defined functions and their instructions
        std::vector< std::vector<uint64_t> > & functions;

        // The current function pointer
        uint64_t currentInstructionBlock;

        // Call stack for the current execution context
        std::stack<uint64_t> callStack;

        // Indicate if a function pointer is on the move
        bool switchingFunction;

        // Built arithmetic operands fiven a particular instruction and type
        void get_arith_lhs_rhs(uint8_t id, uint64_t ins, int64_t * lhs, int64_t * rhs);

        // Attempt to return from the current function
        bool attempt_return();

        // Check if hardware needs to be triggered
        bool hardware_execution_check();

        // Complete a step cycle
        void finalize_cycle();
    };
} 
}


#endif