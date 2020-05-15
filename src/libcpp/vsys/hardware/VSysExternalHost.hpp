#ifndef NABLA_HARDWARE_DEVICE_HOST_HPP
#define NABLA_HARDWARE_DEVICE_HOST_HPP

#include "VSysExternalIf.hpp"

#include <random>

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{

    //! \brief An external 'device' that adds some host-related functionality
    class Host : public ExternalIf 
    {
    public:

        //! \brief Create the external device
        Host();

        //! \brief Destroy the external device
        ~Host();

        // From ExternalIf
        virtual void execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory) override;
    
    private:

        // Marks time when this object is instantiated
        uint64_t start_clock;
    };
}
}
}

#endif