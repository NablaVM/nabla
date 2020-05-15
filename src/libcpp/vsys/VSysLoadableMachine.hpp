#ifndef NABLA_VSYS_LOADABLE_MACHINE
#define NABLA_VSYS_LOADABLE_MACHINE

#include "VSysMachine.hpp"

#include <string>
#include <fstream>

namespace NABLA
{
namespace VSYS
{
    //! \brief A virtual machine implementation that supports being loaded from a
    //!        bytecode file. 
    class LoadableMachine : public Machine
    {
    public:

        //! \brief Result codes returned by loadFile function
        enum class LoadResultCodes
        {
            OKAY,
            UNABLE_TO_OPEN_FILE,
            ERROR_FAILED_TO_LOAD_CONSTANTS,
            ERROR_FAILED_TO_LOAD_FUCNTION,
            ERROR_UNHANDLED_INSTRUCTION,
            ERROR_EOB_NOT_FOUND,
            ERROR_MACHINE_IN_ERROR_STATE
        };

        //! \brief Create a loadable machine
        LoadableMachine();

        //! \brief Detroy a loadable machine
        ~LoadableMachine();

        //! \brief Attempt to populate the machine from file
        //! \param path The path to a file containing bytecode generated from the assembler
        //! \returns A LoadResultCodes enumeration indicating the result of the load
        LoadResultCodes loadFile(std::string path);

    private:
        /*
            Methods for loading VM adapted from libc/binloader
        */
        void load_numerical_constant(FILE* file, uint8_t nBytes, int *result);
        void load_string_constant(FILE* file, int *result);
        int load_constant(FILE * file, int currentByte);
        int load_function(FILE* file);
        int load_end_of_binary(FILE * file);
    };
}
}

#endif 