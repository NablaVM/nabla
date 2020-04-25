/*
    Load and analyze a Nabla binary file
*/

#ifndef HARP_COMPOSER_HPP
#define HARP_COMPOSER_HPP

#include <cstdint>
#include <string>
#include <vector>

extern "C" 
{
    #include "VmCommon.h"
    #include "VmInstructions.h"
}

namespace HARP
{
    typedef struct VM * NablaVirtualMachine;

    //! \class Analyzer
    //! \brief Loads a vm from whatever bin is given and uses that vm to access information about the code
    class Analyzer
    {
    public:

        //! \brief A way to represent a function with its address
        struct FunctionInfo
        {
            uint64_t address;
            std::vector<uint64_t> instructions;
        };

        //! \brief Construct an analyzer
        Analyzer();

        //! \brief Deconstruct an analyzer
        ~Analyzer();

        //! \brief Load a binary
        //! \param file The path to the binary
        //! \retval true The file was loaded 
        //! \retval false The file failed to load
        bool loadBin(std::string file);

        //! \brief Get the contents of the global stack
        //! \returns The contents of the global stack
        //! \pre Binary should be loaded via loadBin
        std::vector<uint64_t> getGlobalStack();

        //! \brief Get the contents of the call stack
        //! \returns The contents of the call stack
        std::vector<uint64_t> getCallStack();

        //! \brief Get the contents of vm registers
        //! \returns The contents of the vm registers
        std::vector<int64_t> getRegisters();

        //! \brief Get the function pointer address
        //! \returns Address of VM function pointer
        uint64_t getFunctionPointer();

        //! \brief Get the VM entry method
        //! \returns The address of the first method to be executed by VM
        uint64_t getEntryAddress();

        //! \brief Get a list of functions within the VM
        //! \returns The functions loaded into the vm along-with all of their instruction
        std::vector<FunctionInfo> getFunctions();

        //! \brief Step the vm processing 'n' cycles
        //! \param n The number of cycles to step
        void step(uint64_t n);

    private:
        bool loaded;
        NablaVirtualMachine vm;
    };
}


#endif