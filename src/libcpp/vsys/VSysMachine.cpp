#include "VSysMachine.hpp"
#include <iostream>

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
        executionContexts.clear();
        externalDeviceMap.clear();
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    bool Machine::attachExternal(uint16_t id, EXTERNAL::ExternalIf &external)
    {
        if(externalDeviceMap.find(id) != externalDeviceMap.end())
        {
            // Already exists
            return false;
        }

        // Set the device
        externalDeviceMap[id] = &external;
        return true;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    bool Machine::addStandardExternalDevices()
    {
        // ADD IO
        if(! attachExternal(NABLA_VSYS_SETTINGS_EXTERNALS_ADDRESS_IO,   external_device_IO) ) { return false; }

        // Add Net
        if(! attachExternal(NABLA_VSYS_SETTINGS_EXTERNALS_ADDRESS_NET,  external_device_Net) ) { return false; }

        // Add Host
        if(! attachExternal(NABLA_VSYS_SETTINGS_EXTERNALS_ADDRESS_HOST, external_device_Host) ) { return false; }

        return true;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
    
    bool Machine::isRunning() const
    {
        return running;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
    
    ExecutionContext * Machine::getExecutionContext(uint64_t id)
    {
        if(executionContexts.size() < id)
        {
            return nullptr;
        }

        return &executionContexts[id];
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

        uint64_t completedContexts = 0;
        for(uint64_t idx = 0; idx < executionContexts.size(); idx++)
        {

            // Its possible that GC hasn't run yet, so we count dead contexts
            if(executionContexts[idx].isContextComplete())
            {
                completedContexts++;
                continue;
            }

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

        // If everything has completed, we die
        if(completedContexts == executionContexts.size())
        {
            running = false;
            return ExecutionReturns::ALL_EXECUTION_COMPLETE;
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

        if(executionContexts.size() == 0)
        {
            running = false;
        }
    }

}
}