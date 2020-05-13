#include "VSysExternalNet.hpp"

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    Net::Net()
    {

    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    Net::~Net()
    {

    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    void Net::execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory)
    {
        std::cout << "NETWORK DEVICE NOT YET IMPLEMENTED" << std::endl;
    }
}
}
}