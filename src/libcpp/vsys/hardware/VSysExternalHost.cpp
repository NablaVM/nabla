#include "VSysExternalHost.hpp"

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    Host::Host()
    {

    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    Host::~Host()
    {

    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    void Host::execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory)
    {
        std::cout << "HOST DEVICE NOT YET IMPLEMENTED" << std::endl;
    }
}
}
}