#include "VSysExternalHost.hpp"

extern "C"
{
    #include "util.h"
    #include <time.h>
}

#include <limits>
#include <random>

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{
    constexpr int NABLA_HOST_DEVICE_CLOCK          =  0;
    constexpr int NABLA_HOST_DEVICE_CLOCKS_PER_SEC =  1;
    constexpr int NABLA_HOST_DEVICE_EPOCH_TIME     = 10;
    constexpr int NABLA_HOST_DEVICE_RANDOM         = 20;

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    Host::Host() : start_clock((uint64_t)clock())
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
        uint8_t subid = util_extract_byte(registers[10], 6);

        switch(subid)
        {
            case NABLA_HOST_DEVICE_CLOCK:
            {
                registers[11] =  (uint64_t)clock() - this->start_clock;
                break;
            }

            case NABLA_HOST_DEVICE_CLOCKS_PER_SEC:
            {
                registers[11] = (uint64_t)CLOCKS_PER_SEC;
                break;
            }

            case NABLA_HOST_DEVICE_EPOCH_TIME:
            {
                registers[11] = time(NULL);
                break;
            }

            case NABLA_HOST_DEVICE_RANDOM:
            {
                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> distribution(0, std::numeric_limits<uint32_t>::max());

                registers[11] = distribution(rng);
                break;
            }
            
            default:
                registers[11] = 0;
                break;
        }

        std::cout << " >> " << registers[11] << std::endl;

        registers[10] = 0;
    }
}
}
}