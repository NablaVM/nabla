#ifndef NABLA_HARDWARE_DEVICE_IO_HPP
#define NABLA_HARDWARE_DEVICE_IO_HPP

#include "VSysExternalIf.hpp"

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{
    class IO : public ExternalIf 
    {
    public:

        IO();
        ~IO();

        // From ExternalIf
        virtual void execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory) override;
    };
}
}
}

#endif