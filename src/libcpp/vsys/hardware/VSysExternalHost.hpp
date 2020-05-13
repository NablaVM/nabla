#ifndef NABLA_HARDWARE_DEVICE_HOST_HPP
#define NABLA_HARDWARE_DEVICE_HOST_HPP

#include "VSysExternalIf.hpp"

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{

    class Host : public ExternalIf 
    {
    public:

        Host();
        ~Host();

        // From ExternalIf
        virtual void execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory) override;
    };
}
}
}

#endif