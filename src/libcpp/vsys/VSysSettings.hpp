#ifndef NABLA_VSYS_SETTINGS_HPP
#define NABLA_VSYS_SETTINGS_HPP

//! \file Settings for the Nabla virtual machine
//! \note While these settings can be changed, it is unwise as it could make the VM no longer
//!       able to execute tested code. (Not enough memory, too many functions, external devices addressed incorrectly, etc)
namespace NABLA 
{
namespace VSYS
{
    // VM Setup information

    constexpr int NABLA_VSYS_SETTINGS_LOCAL_MEMORY_BYTES     = 10000;
    constexpr int NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES    = 10000;
    constexpr int NABLA_VSYS_SETTINGS_MAX_FUNCTIONS          = 100000;

    // Addresses for 'standard' externals that can be loaded by a vm

    constexpr int NABLA_VSYS_SETTINGS_EXTERNALS_ADDRESS_IO   = 0x0A;
    constexpr int NABLA_VSYS_SETTINGS_EXTERNALS_ADDRESS_NET  = 0x0B;
    constexpr int NABLA_VSYS_SETTINGS_EXTERNALS_ADDRESS_HOST = 0x0C;

    
    constexpr int NABLA_SETTINGS_DEVICE_CONFIG_NET_MAX_CONNECTIONS = 100;
}
}

#endif