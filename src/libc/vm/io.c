
#include "io.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NABLA_IO_DEVICE_STDIN     0
#define NABLA_IO_DEVICE_STDOUT    1
#define NABLA_IO_DEVICE_STDERR    2
#define NABLA_IO_DEVICE_DISKIN    3
#define NABLA_IO_DEVICE_DISCKOUT  4
#define NABLA_IO_DEVICE_CLOSE     5
#define NABLA_IO_DEVICE_NONE      6

// --------------------------------------------------------------
//
// --------------------------------------------------------------

struct IODevice * io_new()
{
    struct IODevice * io = (struct IODevice*)malloc(sizeof(struct IODevice));

    assert(io);

    io->target = IODeviceTarget_None;
    io->isDeviceActive = 0;
    io->filePointer = NULL;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void io_delete(struct IODevice * io)
{
    assert(io);

    // Ensure this is closed
    if(NULL != io->filePointer)
    {
        fclose(io->filePointer);
    }

    free(io);
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void io_process(struct IODevice * io, struct VM * vm)
{
    assert(io);
    assert(vm);

    uint8_t target = util_extract_byte(vm->registers[10], 6);

    printf("TARGET: %u\n", target);

    switch (target)
    {
        case NABLA_IO_DEVICE_STDIN   :
        {
            
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: NABLA_IO_DEVICE_STDIN\n");
#endif
            break;
        } 

        case NABLA_IO_DEVICE_STDOUT  :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: NABLA_IO_DEVICE_STDOUT\n");
#endif
            break;
        } 

        case NABLA_IO_DEVICE_STDERR  :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: NABLA_IO_DEVICE_STDERR\n");
#endif
            break;
        } 

        case NABLA_IO_DEVICE_DISKIN  :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: NABLA_IO_DEVICE_DISKIN\n");
#endif
            break;
        } 

        case NABLA_IO_DEVICE_DISCKOUT:
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: NABLA_IO_DEVICE_DISCKOUT\n");
#endif
            break;
        } 

        case NABLA_IO_DEVICE_CLOSE   :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: NABLA_IO_DEVICE_CLOSE\n");
#endif
            break;
        } 

        case NABLA_IO_DEVICE_NONE    :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: NABLA_IO_DEVICE_NONE\n");
#endif
            break;
        } 

        default:
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: Unknown target\n");
#endif
            return;
    }
}





// ------------------------------ OLD IO - Left until iodevice is implemented ----------



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
    char buf;

    uint16_t bytesRead = 0;

    char fullBuffer [NABLA_VM_IO_INPUT_SETTINGS_MAX_IN];

    while(read(STDIN_FILENO, &buf, 1 ) > 0)
    {
        // If newline, end of file
        if(buf == '\n' || buf == EOF)
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_stdin: EOL\n");
#endif
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
    int  shift = 7;
    uint64_t framesProduced = 0;
    uint64_t currentItem = 0;

    // A temp stack to make sure that we store the string in-order and don't need 
    // to do any flip flopping after input
    NablaStack ioStack = stack_new(NABLA_VM_IO_INPUT_SETTINGS_MAX_IN/8);
    assert(ioStack);

    for(uint16_t i = 0; i < bytesRead; i++)
    {
        uint8_t b = (uint8_t)fullBuffer[i];

        currentItem |= ( (uint64_t) b << shift*8 );

        shift--;

        if(shift < 0 || i == bytesRead-1)
        {
            shift = 7;

            int okay = -255;
            stack_push(currentItem, ioStack, &okay);
            assert(okay == STACK_OKAY);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_stdin: Frame : %lu\n", currentItem);
#endif

            currentItem = 0;

            // Inc frames produced so we can indicate how many frames to decompose for data
            framesProduced++;
        }
    }

    // Dump temp stack into global stack. This will ensure that the string is stored in the order that it was
    // received from stdio
    int ioStackOkay = -255;
    int gsStackOkay = -255;
    while(0 == stack_is_empty(ioStack))
    {
        uint64_t val = stack_pop(ioStack, &ioStackOkay);
        assert(ioStackOkay == STACK_OKAY);

        stack_push(val, vm->globalStack, &gsStackOkay);
        assert(gsStackOkay == STACK_OKAY);
    }

    stack_delete(ioStack);

    // Store how many bytes we've read in 
    vm->registers[11] = (uint64_t)bytesRead;

    // Store how many frames we've produced
    vm->registers[12] = framesProduced;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("io_stdin: Bytes read : %lu | Frames composed : %lu\n", vm->registers[11], vm->registers[12]);
#endif

    // Unmark trigger flag
    unset_io(vm);

    return 0;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

int io_out(struct VM * vm, int stream)
{
    // Get the data to write out
    uint64_t out  = vm->registers[11];

    for(int64_t i = 7; i >= 0; i--)
    {
        // Extract each byte of data from output data
        char currentByte = (char)util_extract_byte(out, i);

        // Write that byte
        write(stream, &currentByte, 1);
    }

    // Unmark trigger flag
    unset_io(vm);
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

int io_stdout(struct VM * vm)
{
    return io_out(vm, STDOUT_FILENO);
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

int io_stderr(struct VM * vm)
{
    return io_out(vm, STDERR_FILENO);
}