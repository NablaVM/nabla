/*
    Load and execute a Nabla binary file
*/

#ifndef NABLA_BIN_EXEC_HPP
#define NABLA_BIN_EXEC_HPP

#include <string>

extern "C" 
{
    #include "vm.h"
    #include "VmInstructions.h"
}

namespace NABLA
{
    typedef struct VM * NablaVirtualMachine;

    //! \class BinExec
    //! \brief Leverages binloader to populate a VM and executes the VM
    class BinExec
    {
    public:
        //! \brief Construct a BinExec
        BinExec();

        //! \brief Load a binary
        //! \param file The path to the binary
        //! \retval true The file was loaded 
        //! \retval false The file failed to load
        bool loadBinaryFromFile(std::string file);

        //! \brief Execute the loaded VM
        //! \retval true The program has completed execution with no errors reported by VM
        //! \retval false The program caused an internal VM error (reported by stderr)
        //! \post The BinExec will need to be destroyed and reloaded to execute again.
        bool execute();

    private:
        bool loaded;
        NablaVirtualMachine vm;
    };
}

#endif