#include "io.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int io_stdin(struct VM * vm)
{
    printf("io_stdin\n");

    char buf;

    uint16_t bytesRead = 0;

    char fullBuffer [NABLA_VM_IO_INPUT_SETTINGS_MAX_IN];

    while(read(STDIN_FILENO, &buf, 1 ) > 0)
    {
        // If newline, end of file
        if(buf == '\n' || buf == EOF)
        {
            printf("EOL\n");
            break;
        }

        // Only accept input the size of the buffer
        if(bytesRead < NABLA_VM_IO_INPUT_SETTINGS_MAX_IN)
        {
            fullBuffer[bytesRead] = buf;
            bytesRead++;
        }
    }


    // Pack the bytes into a uint64_t. 
    uint8_t  shift = 0;
    uint64_t framesProduced = 0;
    uint64_t currentItem = 0;

    for(uint16_t i = 0; i < bytesRead; i++)
    {
        uint8_t b = (uint8_t)fullBuffer[i];

        currentItem |= ( (uint64_t) b << shift*8 );

        shift++;

        if(shift > 7 || i == bytesRead-1)
        {
            shift = 0;

            int okay = -255;
            stack_push(currentItem, vm->globalStack, &okay);
            assert(okay == STACK_OKAY);


            printf("Frame : %lu\n", currentItem);


            currentItem = 0;

            // Inc frames produced so we can indicate how many frames to decompose for data
            framesProduced++;
        }
    }

    /*
    
            Might want to rething how we hand off this info.
            Do we store both? Do we store in 'actionable' registers? 
            Are we planning on invoking io with reg 10 still ?
    
    */

    // Store how many bytes we've read in 
    vm->registers[11] = (uint64_t)bytesRead;

    // Store how many frames we've produced
    vm->registers[12] = framesProduced;

    printf("Bytes read : %lu | Frames composed : %lu\n", vm->registers[11], vm->registers[12]);

    unset_io(vm);
}