#ifndef NABLA_HARDWARE_DEVICE_NET_HPP
#define NABLA_HARDWARE_DEVICE_NET_HPP

#include "VSysExternalIf.hpp"

extern "C"
{
    #include "sockpool.h"
}

#include <stdint.h>

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{

    class Net : public ExternalIf 
    {
    public:

        Net();
        ~Net();

        // From ExternalIf
        virtual void execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory) override;

    private:

        uint8_t  active;
        sockpool * socket_pool;
    };
}
}
}

#endif