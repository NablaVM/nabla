#ifndef NABLA_EXTERNAL_IF_HPP
#define NABLA_EXTERNAL_IF_HPP

#include "VSysMemory.hpp"
#include <stdint.h>

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{
    class ExternalIf
    {
    public:
        virtual void execute( int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory) = 0;
    };
}
}
}

#endif