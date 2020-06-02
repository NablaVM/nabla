#ifndef NHLL_CODEGEN_ADDRESS_MANAGER
#define NHLL_CODEGEN_ADDRESS_MANAGER

#include <stdint.h>

namespace NHLL
{
    class AddressManager
    {
    public:
        AddressManager();

        struct AddressResult
        {
            uint64_t address;
            uint64_t  num_bits_assigned;
        };

        
        // Reserve the first n bytes of global stack for system operations
        AddressResult reserve_system_space(uint64_t n);

        // Casts the integer to an int64_t, checks size, and assignes appropriate space
        AddressResult new_global_integer();
        AddressResult new_global_double();
        AddressResult new_global_string(uint64_t length);

        // Called after all globals loaded to create the program's operational stack
        AddressResult mark_stack_start(uint64_t stack_size);

        // A user variable de
        AddressResult new_user_stack_variable(uint64_t byte_size);

    private:
        uint64_t global_address_index;

        // Used to mark user set variables (non-global) of-set from stack start which wont be determiend until finalization
        uint64_t offset_address_index;
    };
}

#endif 