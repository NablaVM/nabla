/*

    Methods beginning with io_* are externally callable methods,
    while process_* are methods used for processing requsts.

*/

#include "io.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NABLA_IO_DEVICE_STDIN     0  
#define NABLA_IO_DEVICE_STDOUT    1  
#define NABLA_IO_DEVICE_STDERR    2  
#define NABLA_IO_DEVICE_DISKIN    100
#define NABLA_IO_DEVICE_DISCKOUT  101
#define NABLA_IO_DEVICE_CLOSE     200
#define NABLA_IO_DEVICE_NONE      250

#define NABLA_IO_DEVICE_DISKIN_OPEN    1
#define NABLA_IO_DEVICE_DISKIN_READ   10
#define NABLA_IO_DEVICE_DISKIN_SEEK   20
#define NABLA_IO_DEVICE_DISKIN_REWIND 30
#define NABLA_IO_DEVICE_DISKIN_TELL   40

#define NABLA_IO_DEVICE_DISKOUT_OPEN  1
#define NABLA_IO_DEVICE_DISKOUT_WRITE 10



// --------------------------------------------------------------
//
// --------------------------------------------------------------

struct IODevice * io_new()
{
    struct IODevice * io = (struct IODevice*)malloc(sizeof(struct IODevice));

    assert(io);

    io->target = IODeviceTarget_Close;
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
// Unset activation register, and close out the device
// --------------------------------------------------------------

void process_close_io(struct IODevice * io, struct VM * vm)
{
    vm->registers[10] = 0;

    // Indicate we io done
    io->target = IODeviceTarget_Close;
    io->isDeviceActive = 0;

    // If they were using file pointer, close it
    if(NULL != io->filePointer)
    {
        fclose(io->filePointer);
    }
}

// --------------------------------------------------------------
//  Unset activation register
// --------------------------------------------------------------

void process_unset_io(struct VM * vm)
{
    vm->registers[10] = 0;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_stdin(struct IODevice * io, struct VM * vm)
{
    if(io->target != IODeviceTarget_Close)
    {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        printf("Request for stdin failed. IO Device in-use\n");
        return;
#endif
    }

    // Get the number of bytes to read in
    uint32_t bytesToRead = util_extract_two_bytes(vm->registers[10], 5) << 16;
    bytesToRead |= util_extract_two_bytes(vm->registers[10], 3);

    // Get the termination byte
    uint8_t term = util_extract_byte(vm->registers[10], 1);

    // Buf to read a byte into
    char buf;

    // Number of bytes read
    uint16_t bytesRead = 0;

    // The full buffer of data
    char * fullBuffer = (char *)malloc(sizeof(char) * bytesToRead);

    assert(fullBuffer);

    // Read byte by byte until TERM or max bytes read
    while(read(STDIN_FILENO, &buf, 1 ) > 0)
    {
        // If TERM or EOF
        if((uint8_t)buf == term || buf == EOF)
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("process_stdin: TERM\n");
#endif
            break;
        }

        // Only accept input the size of the buffer
        if(bytesRead < bytesToRead)
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
    NablaStack ioStack = stack_new(bytesToRead/8);
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

    // Unmark trigger flag
    process_close_io(io, vm);
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_io_out(struct IODevice * io, struct VM * vm, int stream)
{
    if(io->target != IODeviceTarget_Close)
    {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        printf("Request for std(out|err) failed. IO Device in-use\n");
        return;
#endif
    }

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
    process_close_io(io, vm);
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

char * process_build_file_name(struct VM * vm)
{
    uint32_t startAddress  = util_extract_two_bytes(vm->registers[11], 7) << 16;
                startAddress |= util_extract_two_bytes(vm->registers[11], 5);

    uint32_t endAddress  = util_extract_two_bytes(vm->registers[11], 3) << 16;
                endAddress |= util_extract_two_bytes(vm->registers[11], 1);

    uint64_t fileNameSize = ((endAddress - startAddress) * 8);

    char * fileNameBuf = (char *)malloc(sizeof(char) * fileNameSize);
    assert(fileNameBuf);

    uint64_t fileNameIdx = 0;

    for(uint64_t i = startAddress; i < endAddress; i++)
    {
        int res = -255;
        uint64_t curr = stack_value_at(i, vm->globalStack, &res);
        assert(res == STACK_OKAY);

        for(int i = 7; i >= 0; i--)
        {
            uint8_t c = curr >> i * 8;
            fileNameBuf[fileNameIdx] = (char)c;
            fileNameIdx++;
        }
    }

    return fileNameBuf;
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_io_disk_in(struct IODevice * io, struct VM * vm)
{
    uint8_t instruction = util_extract_byte(vm->registers[10], 5);

    switch(instruction)
    {
        case NABLA_IO_DEVICE_DISKIN_OPEN  :
        {
            // Ensure that the device is closed before we continue
            if(io->target != IODeviceTarget_Close)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            // Get the file name - if it fails the system will die!!
            char * fileNameBuf = process_build_file_name(vm);

            printf("Loaded file name : %s\n", fileNameBuf);

            // Open the io filePointer
            io->filePointer = fopen(fileNameBuf, "r");

            // We don't need this anymore
            free(fileNameBuf);

            // If open fails, indicate it
            if(io->filePointer == NULL)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            // Unset, but don't close
            process_unset_io(vm);
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_READ  :
        {
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_SEEK  :
        {
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_REWIND:
        {
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_TELL  :
        {
            break;
        }
        default:
            // Illegal instruction
            vm->registers[11] = 0;
            process_close_io(io, vm);
            break;
    }


    // Check if command is to open
    // If command is to open, ensure that the IO device is currently closed. Otherwise bail out
        // Read the stack start and end address. Attempt to open. If opened, place 1 in r11, otherwise place 0, 
        // and unset device to be closed
        // Return

    // If command isn't open, ensure that the IO device is already in DiskIn mode. Otherwise bail out

    // If read - Get num bytes to read from instruction. Read those bytes into the stack
    // If seek - Get seek location from isntruction. Seek. If fail r11 = 0 else r11 = 1
    // If rewind - Rewind. If fail r11 = 0 else r11 = 1
    // If tell - Place result value in r11
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_io_disk_out(struct IODevice * io, struct VM * vm)
{
    uint8_t instruction = util_extract_byte(vm->registers[10], 5);
    uint8_t mode        = util_extract_byte(vm->registers[10], 4);

    printf("process_io_disk_out\n");

    switch(instruction)
    {
        case NABLA_IO_DEVICE_DISKOUT_OPEN:
        {
            // Ensure that the device is closed before we continue
            if(io->target != IODeviceTarget_Close)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

             // Get the file name - if it fails the system will die!!
            char * fileNameBuf = process_build_file_name(vm);

            // Open the io filePointer
            io->filePointer = fopen(fileNameBuf, "w");

            // We don't need this anymore
            free(fileNameBuf);

            // If open fails, indicate it
            if(io->filePointer == NULL)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            // Indicate that we are doing disk out
            io->target = IODeviceTarget_DiskOut;

            // Indicate success
            vm->registers[11] = 1;

            // Unset, but don't close
            process_unset_io(vm);
            break;
        }

        case NABLA_IO_DEVICE_DISKOUT_WRITE:
        {
            // Ensure that the device is closed before we continue
            if(io->target != IODeviceTarget_DiskOut)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            // Write each byte in the register
            for(int i = 7; i >= 0; i--)
            {
                uint8_t c =  util_extract_byte(vm->registers[11], i);
                fputc((char)c, io->filePointer);
            }

            vm->registers[11] = 1;

            // Indicate write complete
            process_unset_io(vm);
            break;
        }

        default:
            vm->registers[11] = 0;
            process_close_io(io, vm);
            break;
    }
}

// --------------------------------------------------------------
//  Entry point for IO process
// --------------------------------------------------------------

void io_process(struct IODevice * io, struct VM * vm)
{
    assert(io);
    assert(vm);

    // Indicate that the 'device' is active
    io->isDeviceActive = 1;

    // Get the target
    uint8_t target = util_extract_byte(vm->registers[10], 6);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("TARGET: %u\n", target);
#endif

    // Execute base on target
    switch (target)
    {
        case NABLA_IO_DEVICE_STDIN   :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_stdin\n");
#endif
            process_stdin(io, vm);
            break;
        } 

        case NABLA_IO_DEVICE_STDOUT  :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_stdout\n");
#endif
            process_io_out(io, vm, STDOUT_FILENO);
            break;
        } 

        case NABLA_IO_DEVICE_STDERR  :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_stderr\n");
#endif
            process_io_out(io, vm, STDERR_FILENO);
            break;
        } 

        case NABLA_IO_DEVICE_DISKIN  :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("process_io_disk_in\n");
#endif
            process_io_disk_in(io, vm);
            break;
        } 

        case NABLA_IO_DEVICE_DISCKOUT:
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("process_io_disk_out\n");
#endif
            process_io_disk_out(io, vm);
            break;
        } 

        case NABLA_IO_DEVICE_CLOSE   :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("process_close_io\n");
#endif
            printf("CLOSE\n");
            process_close_io(io, vm);
            break;
        } 

        default:
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: Unknown target\n");
#endif
            return;
    }
}