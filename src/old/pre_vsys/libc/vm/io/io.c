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

#define NABLA_IO_DEVICE_STDIN        0  
#define NABLA_IO_DEVICE_STDOUT       1  
#define NABLA_IO_DEVICE_STDERR       2  
#define NABLA_IO_DEVICE_DISKIN       100
#define NABLA_IO_DEVICE_DISCKOUT     101
#define NABLA_IO_DEVICE_CLOSE        200
#define NABLA_IO_DEVICE_REPORT       255
#define NABLA_IO_DEVICE_DISKIN_OPEN    1
#define NABLA_IO_DEVICE_DISKIN_READ   10
#define NABLA_IO_DEVICE_DISKIN_SEEK   20
#define NABLA_IO_DEVICE_DISKIN_REWIND 30
#define NABLA_IO_DEVICE_DISKIN_TELL   40
#define NABLA_IO_DEVICE_DISKOUT_OPEN  1
#define NABLA_IO_DEVICE_DISKOUT_WRITE 10

// Different states for the IO Device
enum IODeviceState
{
    IODeviceState_Stdin,    // Standard input
    IODeviceState_Stdout,   // Standard output
    IODeviceState_Stderr,   // Standard error
    IODeviceState_DiskIn,   // Disk input
    IODeviceState_DiskOut,  // Disk output
    IODeviceState_Close,    // Close state
};

// The input / output stream handler
struct IODevice
{
    enum    IODeviceState state;  // Target specified 
    FILE *  filePointer;            // File pointer for device
    int      gsByteIndex;
};

// --------------------------------------------------------------
//
// --------------------------------------------------------------

struct IODevice * io_new()
{
    struct IODevice * io = (struct IODevice*)malloc(sizeof(struct IODevice));

    assert(io);

    io->state = IODeviceState_Close;
    io->filePointer = NULL;
    io->gsByteIndex  = 0;
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
    io->state = IODeviceState_Close;

    io->gsByteIndex  = 0;

    // If they were using file pointer, close it
    if(NULL != io->filePointer)
    {
        fclose(io->filePointer);
        io->filePointer = NULL;
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
    if(io->state != IODeviceState_Close)
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

    for(uint16_t i = 0; i < bytesRead; i++)
    {
        uint8_t b = (uint8_t)fullBuffer[i];

        currentItem |= ( (uint64_t) b << shift*8 );

        shift--;

        if(shift < 0 || i == bytesRead-1)
        {
            shift = 7;

            int okay = -255;
            stack_push(currentItem, vm->globalStack, &okay);
            assert(okay == STACK_OKAY);

            currentItem = 0;

            // Inc frames produced so we can indicate how many frames to decompose for data
            framesProduced++;
        }
    }

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
    if(io->state != IODeviceState_Close)
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
            if(io->state != IODeviceState_Close)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            // Get the file name - if it fails the system will die!!
            char * fileNameBuf = process_build_file_name(vm);

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

            // Indicate that we are doing disk in
            io->state = IODeviceState_DiskIn;

            // Setup byte indexing
            io->gsByteIndex  = 7;

            // Indicate success
            vm->registers[11] = 1;

            // Unset, but don't close
            process_unset_io(vm);
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_READ  :
        {
            // Ensure that the device is open before we continue
            if(io->state != IODeviceState_DiskIn)
            {
                vm->registers[11] = 0;  // For '0' bytes read in - also because the thing isn't open
                process_close_io(io, vm);
                return;
            }

            uint32_t numBytesToRead  = util_extract_two_bytes(vm->registers[10], 4) << 16;
                     numBytesToRead |= util_extract_two_bytes(vm->registers[10], 2);
        
            uint64_t bytesReadIn = 0;
            uint64_t framesProduced = 0;
            for(uint32_t i = 0; i < numBytesToRead; i++)
            {
                // If we need to reset the byte index (This could be done below, but i feel its more clear here)
                if(io->gsByteIndex < 0)
                {
                    io->gsByteIndex = 7;
                }

                // We don't else if here so we can handle the cases of first-time loop
                if(io->gsByteIndex == 7)
                {
                    // Need a new frame to put things in
                    int newFrameOkay = -255;
                    stack_push(0, vm->globalStack, &newFrameOkay);
                    assert(newFrameOkay == STACK_OKAY);

                    // Inc frames produced
                    framesProduced++;
                }
                
                // Get the stack frame that we are operating on
                uint64_t frameIdx = stack_get_size(vm->globalStack);

                // Should be size-1, but 0 check for sanity and safety
                if(frameIdx != 0) frameIdx --;

                uint8_t currentByte;

                if(1 == fread(&currentByte, 1, 1, io->filePointer))
                {
                    bytesReadIn++;

                    // Get the current data in the stack at the frame index
                    int valAtOkay = -255;
                    uint64_t theCurrentFrame = stack_value_at(frameIdx, vm->globalStack, &valAtOkay);
                    assert(valAtOkay == STACK_OKAY);

                    // Concat the current byte to it
                    theCurrentFrame |= ( (uint64_t)currentByte << io->gsByteIndex * 8 );

                    // Store it back where it belongs
                    int storeOkay = -255;
                    stack_set_value_at(frameIdx, theCurrentFrame, vm->globalStack, &storeOkay);
                    assert(storeOkay == STACK_OKAY);

                    // Subtract the byte index so we know how much to shift the next time for concat
                    io->gsByteIndex--;
                }
                else
                {
                    // If a fail to read happens, 
                    vm->registers[11] = i;
                    vm->registers[12] = framesProduced;
                    process_unset_io(vm);
                    return;
                }
            } // End for loop

            vm->registers[11] = bytesReadIn;
            vm->registers[12] = framesProduced;
            process_unset_io(vm);
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_SEEK  :
        {
            // Ensure that the device is open before we continue
            if(io->state != IODeviceState_DiskIn)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            uint32_t seekLoc  = util_extract_two_bytes(vm->registers[10], 4) << 16;
                     seekLoc |= util_extract_two_bytes(vm->registers[10], 2);

            if( 0 == fseek( io->filePointer , seekLoc, SEEK_SET ) )
            {
                vm->registers[11] = 1;
            }
            else
            {
                vm->registers[11] = 0;
            }

            process_unset_io(vm);
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_REWIND:
        {
            // Ensure that the device is open before we continue
            if(io->state != IODeviceState_DiskIn)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            rewind(io->filePointer);
            vm->registers[11] = 1;
            process_unset_io(vm);
            break;
        }
        case NABLA_IO_DEVICE_DISKIN_TELL  :
        {
            // Ensure that the device is open before we continue
            if(io->state != IODeviceState_DiskIn)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

            vm->registers[11] = ftell(io->filePointer);
            process_unset_io(vm);
            break;
        }
        default:
            // Illegal instruction
            vm->registers[11] = 0;
            process_close_io(io, vm);
            break;
    }
}

// --------------------------------------------------------------
//
// --------------------------------------------------------------

void process_io_disk_out(struct IODevice * io, struct VM * vm)
{
    uint8_t instruction = util_extract_byte(vm->registers[10], 5);
    uint8_t mode        = util_extract_byte(vm->registers[10], 4);

    switch(instruction)
    {
        case NABLA_IO_DEVICE_DISKOUT_OPEN:
        {
            // Ensure that the device is closed before we continue
            if(io->state != IODeviceState_Close)
            {
                vm->registers[11] = 0;
                process_close_io(io, vm);
                return;
            }

             // Get the file name - if it fails the system will die!!
            char * fileNameBuf = process_build_file_name(vm);

            if(mode == 1)
            {
                // Open the io filePointer as write
                io->filePointer = fopen(fileNameBuf, "w");
            }
            else if (mode == 2)
            {
                // Open the io filePointer as append
                io->filePointer = fopen(fileNameBuf, "a");
            }
            else if (mode == 3)
            {
                // Open the io filePointer as append
                io->filePointer = fopen(fileNameBuf, "a+");
            }

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
            io->state = IODeviceState_DiskOut;

            // Indicate success
            vm->registers[11] = 1;

            // Unset, but don't close
            process_unset_io(vm);
            break;
        }

        case NABLA_IO_DEVICE_DISKOUT_WRITE:
        {
            // Ensure that the device is closed before we continue
            if(io->state != IODeviceState_DiskOut)
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
//
// --------------------------------------------------------------

void process_report(struct IODevice * io, struct VM * vm)
{
    switch(io->state)
    {
        case IODeviceState_Stdin   : vm->registers[11] = NABLA_IO_DEVICE_STDIN   ; break;
        case IODeviceState_Stdout  : vm->registers[11] = NABLA_IO_DEVICE_STDOUT  ; break;
        case IODeviceState_Stderr  : vm->registers[11] = NABLA_IO_DEVICE_STDERR  ; break;
        case IODeviceState_DiskIn  : vm->registers[11] = NABLA_IO_DEVICE_DISKIN  ; break;
        case IODeviceState_DiskOut : vm->registers[11] = NABLA_IO_DEVICE_DISCKOUT; break;
        case IODeviceState_Close   : vm->registers[11] = NABLA_IO_DEVICE_CLOSE   ; break;
    }
    process_unset_io(vm);
}

// --------------------------------------------------------------
//  Entry point for IO process
// --------------------------------------------------------------

void io_process(struct IODevice * io, struct VM * vm)
{
    assert(io);
    assert(vm);

    // Get the target
    uint8_t target = util_extract_byte(vm->registers[10], 6);

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
            process_close_io(io, vm);
            break;
        } 

        case NABLA_IO_DEVICE_REPORT :
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_report\n");
#endif
            process_report(io,vm);
            break;
        }

        default:
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("io_process: Unknown target\n");
#endif
            process_close_io(io, vm);
            return;
    }
}