#ifndef HARP_COMPOSER_HPP
#define HARP_COMPOSER_HPP

#include <string>

extern "C" 
{
    #include "VmCommon.h"
    #include "VmInstructions.h"
}

typedef struct VM * NablaVirtualMachine;

namespace HARP
{
    //! \class Analyzer
    //! \brief Loads a vm from whatever bin is given and uses that vm to access information about the code
    class Analyzer
    {
    public:
        //! \brief Construct an analyzer
        Analyzer();

        //! \brief Load a binary
        //! \param file The path to the binary
        //! \retval true The file was loaded 
        //! \retval false The file failed to load
        bool loadBin(std::string file);
        
    private:
        NablaVirtualMachine vm;
    };
}


#endif