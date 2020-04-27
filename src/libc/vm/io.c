#include "io.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void unset_io(struct VM * vm)
{
    vm->registers[10] = 0;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

int io_input(struct VM * vm)
{
    switch(vm->registers[10])
    {
        case NABLA_VM_IO_INPUT_READ_ONE:
        {
            char ch;
            scanf("%c", &ch);

            unset_io(vm);

            int okay = -255;
            stack_push((uint64_t)ch, vm->globalStack, &okay);

            assert(okay == STACK_OKAY);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("NABLA_VM_IO_INPUT_READ_ONE: %c\n", ch);
#endif
            break;
        }
        case NABLA_VM_IO_INPUT_READ_N:
        {
            // Get the expected size
            uint64_t size = vm->registers[11];

            // Ensure it isn't greater than the max allowed
            assert(size < NABLA_VM_IO_INPUT_SETTINGS_MAX_IN);

            // Create a string
            char str[NABLA_VM_IO_INPUT_SETTINGS_MAX_IN];

            // Get the input from the user
            scanf("%[^\t\n]s", str);

/*

    TODO: This should pack the chars into each frame so a single char doesn't take up
          8 whole bytes. 

*/

            // Dump string into global stack
            for(uint64_t j = 0; j < size; j++)
            {
                int okay = -255;
                stack_push((uint64_t)str[j], vm->globalStack, &okay);
                assert(okay == STACK_OKAY);
            }

            // Indicate that we have done the requested io
            unset_io(vm);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("NABLA_VM_IO_INPUT_READ_N [%lu] : %s\n", size, str);
#endif
            break;
        }
        case NABLA_VM_IO_INPUT_READ_INT:
        {
            // Create a string
            char *strp;
            char str[NABLA_VM_IO_INPUT_SETTINGS_MAX_IN];
            int64_t n;

            fgets(str, sizeof(str), stdin);

            // Convert to long, base 10
            n = strtol(str, &strp, 10);

            // Check to ensure is valid, if not, 0
            if (strp == str || *strp != '\n') 
            {
                n = 0;
            }
                
            int okay = -255;
            stack_push((uint64_t)n, vm->globalStack, &okay);
            assert(okay == STACK_OKAY);

            // Indicate that we have done the requested io
            unset_io(vm);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("NABLA_VM_IO_INPUT_READ_INT : %lu\n", (uint64_t)n);
#endif
            break;
        }
        case NABLA_VM_IO_INPUT_READ_DOUBLE:
        {
            char *strp;
            char str[NABLA_VM_IO_INPUT_SETTINGS_MAX_IN];
            double n;
 
            fgets(str, sizeof(str), stdin);

            // Convert to long
            n = strtod(str, &strp);

            // Check to ensure is valid, if not, 0
            if (strp == str || *strp != '\n') 
            {
                n = 0;
            }

            union deval
            {
                uint64_t val;
                double d;
            };

            union deval d; d.d = n;

            int okay = -255;
            stack_push(d.val, vm->globalStack, &okay);
            assert(okay == STACK_OKAY);

            unset_io(vm);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("NABLA_VM_IO_INPUT_READ_DOUBLE : %lu\n", d.val);
#endif
            break;
        }
    }
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

int io_ouput(struct VM * vm)
{
    switch(vm->registers[10])
    {
        case NABLA_VM_IO_OUPUT_SIGNED_INT :
        {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("\n");
#endif
            break;
        }
        case NABLA_VM_IO_OUPUT_UNSIGNED_DEC :
        {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("\n");
#endif
            break;
        }
        case NABLA_VM_IO_OUPUT_UNSIGNED_HEX :
        {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("\n");
#endif
            break;
        }
        case NABLA_VM_IO_OUPUT_FLOATIN_POINT :
        {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("\n");
#endif
            break;
        }
        case NABLA_VM_IO_OUPUT_MIN_FLOATING_POINT :
        {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("\n");
#endif
            break;
        }
        case NABLA_VM_IO_OUPUT_CHAR :
        {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("\n");
#endif
            break;
        }
        case NABLA_VM_IO_OUPUT_STRING :
        {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("\n");
#endif
            break;
        }
    }
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------


int io_invoke(struct VM * vm)
{
    printf("io invoked\n");

    if(vm->registers[10] >= NABLA_VM_IO_OUPUT_SIGNED_INT)
    {
        return io_ouput(vm);
    }
    else
    {
        return io_input(vm);
    }
}