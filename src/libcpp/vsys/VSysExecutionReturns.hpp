#ifndef NABLA_VSYS_EXCECUTION_RETURNS_HPP
#define NABLA_VSYS_EXCECUTION_RETURNS_HPP

namespace NABLA
{
namespace VSYS
{
    enum class ExecutionReturns
    {
        OKAY,
        ALL_EXECUTION_COMPLETE,
        INSTRUCTION_NOT_FOUND,
        UNKNOWN_INSTRUCTION,
        FAILED_TO_SPAWN_EXECUTION_CONTEXT,
        EXECUTION_ERROR
    };
} 
}


#endif