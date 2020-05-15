#ifndef NABLA_HARDWARE_DEVICE_IO_HPP
#define NABLA_HARDWARE_DEVICE_IO_HPP

#include "VSysExternalIf.hpp"

#include <stdio.h>

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{
    //! \brief An external 'device' that adds IO functionality
    class IO : public ExternalIf 
    {
    public:

        //! \brief Create the external device
        IO();

        //! \brief Destroy the external device
        ~IO();

        // From ExternalIf
        virtual void execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory) override;

    private:
    
        // Save device state from one operation to the next by preserving 
        // the hidden device enumeration
        int     state;

        // Uses C-Style as the previous implementation was written in C, and this
        // object acts as a wrapper for the previous implementation
        FILE *  filePointer;
    };
}
}
}

#endif