#include "AddressManager.hpp"
#include <limits>

#include <iostream>

namespace NHLL
{
    AddressManager::AddressManager() : global_address_index(0), offset_address_index(1)
    {

    }

    AddressManager::AddressResult AddressManager::reserve_system_space(uint64_t n)
    {
        uint64_t  assigned = n * 8;
        uint64_t addr = global_address_index;

        global_address_index += n;
        
        return AddressResult { addr, assigned };
    }

    AddressManager::AddressResult AddressManager::new_global_integer()
    {
        // For now we are setting all global integers to int64s
        uint64_t  assigned = 0;
        uint64_t addr = global_address_index;

        assigned = 64;
        global_address_index += 8;

        return AddressResult { addr, assigned };
    }

    AddressManager::AddressResult AddressManager::new_global_double()
    {
        uint64_t addr = global_address_index;
        global_address_index += 8;
        return AddressResult{ addr, 64 };
    }

    AddressManager::AddressResult AddressManager::new_global_string(uint64_t length)
    {
        uint64_t addr = global_address_index;
        global_address_index += length;
        return AddressResult{ addr, length * 8};
    }

    AddressManager::AddressResult AddressManager::mark_stack_start(uint64_t stack_size)
    {
        uint64_t addr = global_address_index;
        global_address_index += stack_size;
        return AddressResult{ addr, stack_size * 8};
    }

    AddressManager::AddressResult AddressManager::new_user_stack_variable(uint64_t byte_size)
    {
        uint64_t addr = offset_address_index;
        offset_address_index += byte_size;
        return AddressResult{ addr, byte_size * 8};
    }
}