#ifndef NABLA_VSYS_MACHINE_HPP
#define NABLA_VSYS_MACHINE_HPP

#include "VSysSettings.hpp"
#include "VSysMemory.hpp"
#include "VSysExecutionContext.hpp"
#include "VSysExecutionReturns.hpp"
#include "VSysInstructions.hpp"

// External Device stuff
#include "VSysExternalIf.hpp"
#include "VSysExternalIO.hpp"
#include "VSysExternalNet.hpp"
#include "VSysExternalHost.hpp"

#include <stdint.h>
#include <vector>
#include <map>

namespace NABLA
{
namespace VSYS
{
    //! \brief Base 'Machine' class of which virual machine implementations can
    //!        use to define particular VM instances
    class Machine
    {
    public:

        //! \brief Creates a Machine
        Machine();

        //! \brief Destructs a Machine
        ~Machine();

        //! \brief Attach an external device that can be triggered by instructions in the VM
        //!        The external device needs to have a unique id (see VSysSettings.hpp for in-use addresses)
        //!        The device will have access to the execution context registers, and global memory.
        //!        The device will be triggered by the presence of it's id in register 10, After being called
        //!        Register 10 will be cleared, but other data will persist. Registers 10-15 are reserved by
        //!        convention for device operation
        //! \param id The ID that will map execution to the device
        //! \param externalDevice A reference to an external device that will be placed into the device map
        bool attachExternal(uint16_t id, EXTERNAL::ExternalIf &externalDevice);

        //! \brief Step the execution for all operational contexts
        //! \param stepsPerContext The number of ticks that each existing and active context will be stepped 
        //! \returns ExecutionReturns enumeration indicating the result of the execution cycle
        ExecutionReturns step(uint64_t stepsPerContext);

        //! \brief Clean up completed execution contexts.
        //!        Once a context completes it is marked as inactive, but it is not implicitly removed. As doing
        //!        so would be the base machine making implementation decisions. The implementation, or external
        //!        entity is in charge of triggering the clean up of completed contexts
        //! \post  Memory for completed contexts will be released
        void executionContextGarbageCollection();

        //! \brief Check if the VM is running
        //! \returns True if running, False otherwise
        bool isRunning() const;

        //! \brief Retrieve an execution context
        //! \param id The ID of the execution context
        //! \returns nullptr If no context exists at that id.
        //! \note This is useful for testing and maybe some fancy magic, but should be avoided
        //!       unless you are sure about what you're doing. The pointer could end up being 
        //!       useless if the context closes post-step 
        ExecutionContext * getExecutionContext(uint64_t id);
    
    protected:

        //! \brief Add the standard external processing virtual devices to machine
        //!        This is optional for the child class, though code relying on standard
        //!        devices wont work properly if they are not added.
        //! \retval True devices added, False, devices failed to get added
        bool addStandardExternalDevices();

        // Flag for if the VM is running
        bool running;

        // Something happened, and now the VM can't continue
        bool inErrorState;

        // Address of first function to execute
        uint64_t entryAddress;

        // Number of functions existing within the VM 
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

        // External devices
        std::map<uint16_t, EXTERNAL::ExternalIf*> externalDeviceMap;

        // Create a new execution context (called at start of machine by machine, and triggered
        // by a currently running execution context)
        bool newExecutionContext(uint64_t address);

        // Standard external processing 'devices' 

        EXTERNAL::IO   external_device_IO;
        EXTERNAL::Net  external_device_Net;
        EXTERNAL::Host external_device_Host;
    };
} 
}


#endif