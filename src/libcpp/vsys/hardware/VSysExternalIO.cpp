/*
    So, I cheated. 
    I wrote all of this in C for the original VM. I REALLLLLY didn't want to rewrite the whole thing...
    So I plopped the C functions here, and preserved the parameters they were passed by making the structs they
    expected out of the data that the the new VM has (global memory, registers, etc)
    Once the individual methods are completed, the class takes the state information from the IO struct and updates
    its self. 
    If anyone other than me is reading this, I really wouldn't mind if you decided to redo this in C++ :)
*/

#include "VSysExternalIO.hpp"

extern "C"
{
    #include "util.h"
    #include <assert.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
}

#include <string>

namespace NABLA
{
namespace VSYS
{
namespace EXTERNAL
{
    constexpr int NABLA_IO_DEVICE_STDIN         =   0;
    constexpr int NABLA_IO_DEVICE_STDOUT        =   1;
    constexpr int NABLA_IO_DEVICE_STDERR        =   2;
    constexpr int NABLA_IO_DEVICE_DISKIN        = 100;
    constexpr int NABLA_IO_DEVICE_DISCKOUT      = 101;
    constexpr int NABLA_IO_DEVICE_CLOSE         = 200;
    constexpr int NABLA_IO_DEVICE_REPORT        = 255;
    constexpr int NABLA_IO_DEVICE_DISKIN_OPEN   =   1;
    constexpr int NABLA_IO_DEVICE_DISKIN_READ   =  10;
    constexpr int NABLA_IO_DEVICE_DISKIN_SEEK   =  20;
    constexpr int NABLA_IO_DEVICE_DISKIN_REWIND =  30;
    constexpr int NABLA_IO_DEVICE_DISKIN_TELL   =  40;
    constexpr int NABLA_IO_DEVICE_DISKOUT_OPEN  =   1;
    constexpr int NABLA_IO_DEVICE_DISKOUT_WRITE =  10;

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
    };

    struct VM
    {
        Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory;
        int64_t (&registers)[16];
    };

    /*
    
            Functions from old io 
    
    */

    // --------------------------------------------------------------
    // Unset activation register, and close out the device
    // --------------------------------------------------------------

    void process_close_io(struct IODevice * io, struct VM * vm)
    {
        vm->registers[10] = 0;

        // Indicate we io done
        io->state = IODeviceState_Close;

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

    std::string process_build_file_name(struct VM * vm)
    {
        uint32_t startAddress  = util_extract_two_bytes(vm->registers[11], 7) << 16;
                 startAddress |= util_extract_two_bytes(vm->registers[11], 5);

        uint32_t endAddress  = util_extract_two_bytes(vm->registers[11], 3) << 16;
                 endAddress |= util_extract_two_bytes(vm->registers[11], 1);

        uint64_t fileNameSize = ((endAddress - startAddress));

        std::string fileName = "";
        for(uint64_t i = startAddress; i < endAddress; i++)
        {
            uint8_t v = 0;
            bool result = vm->global_memory.get_8(i, v);

            assert(result);

            fileName += (char)v;
        }

        return fileName;
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
                std::string fileNameBuf = process_build_file_name(vm);

                // Open the io filePointer
                io->filePointer = fopen(fileNameBuf.c_str(), "r");

                // If open fails, indicate it
                if(io->filePointer == NULL)
                {
                    vm->registers[11] = 0;
                    process_close_io(io, vm);
                    return;
                }

                // Indicate that we are doing disk in
                io->state = IODeviceState_DiskIn;

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

                for(uint32_t i = 0; i < numBytesToRead; i++)
                {
                    uint8_t currentByte;

                    if(1 == fread(&currentByte, 1, 1, io->filePointer))
                    {
                        bytesReadIn++;

                        bool res = vm->global_memory.push_8(currentByte);
                        assert(res);
                    }

                } // End for loop

                vm->registers[11] = bytesReadIn;
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
                std::string fileNameBuf = process_build_file_name(vm);

                if(mode == 1)
                {
                    // Open the io filePointer as write
                    io->filePointer = fopen(fileNameBuf.c_str(), "w");
                }
                else if (mode == 2)
                {
                    // Open the io filePointer as append
                    io->filePointer = fopen(fileNameBuf.c_str(), "a");
                }
                else if (mode == 3)
                {
                    // Open the io filePointer as append
                    io->filePointer = fopen(fileNameBuf.c_str(), "a+");
                }

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

            if(bytesRead < bytesToRead)
            {
                bool result = vm->global_memory.push_8(static_cast<uint8_t>(buf));
                assert(result);
                bytesRead++;
            }
        }

        // Store how many bytes we've read in 
        vm->registers[11] = (uint64_t)bytesRead;

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
            if(-1 == write(stream, &currentByte, 1))
            {
                process_close_io(io, vm);
                return;
            }
        }

        // Unmark trigger flag
        process_close_io(io, vm);
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

    /*
    
            New IO Mechanism Adapter
    
    */

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    IO::IO() : state(static_cast<int>(IODeviceState_Close)), filePointer(nullptr)
    {

    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    IO::~IO()
    {

    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    void IO::execute(int64_t (&registers)[16], Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory)
    {

        // Build the structs that the previous implementation used 

        VM vm = { global_memory, registers };

        IODevice io = 
        { 
            static_cast<IODeviceState>(this->state), 
            this->filePointer
        };

        // Get the target
        uint8_t target = util_extract_byte(registers[10], 6);

        // Execute base on target
        switch (target)
        {
            case NABLA_IO_DEVICE_STDIN   :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("io_stdin\n");
#endif
                process_stdin(&io, &vm);
                break;
            } 

            case NABLA_IO_DEVICE_STDOUT  :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("io_stdout\n");
#endif
                process_io_out(&io, &vm, STDOUT_FILENO);
                break;
            } 

            case NABLA_IO_DEVICE_STDERR  :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
               printf("io_stderr\n");
#endif
                process_io_out(&io, &vm, STDERR_FILENO);
                break;
            } 

            case NABLA_IO_DEVICE_DISKIN  :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("process_io_disk_in\n");
#endif
                process_io_disk_in(&io, &vm);
                break;
            } 

            case NABLA_IO_DEVICE_DISCKOUT:
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("process_io_disk_out\n");
#endif
                process_io_disk_out(&io, &vm);
                break;
            } 

            case NABLA_IO_DEVICE_CLOSE   :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("process_close_io\n");
#endif
                process_close_io(&io, &vm);
                break;
            } 

            case NABLA_IO_DEVICE_REPORT :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("io_report\n");
#endif
                process_report(&io, &vm);
                break;
            }

            default:
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("io_process: Unknown target\n");
#endif
                process_close_io(&io, &vm);
                return;
        }

        // Save state info
        this->state = static_cast<int>(io.state);
        this->filePointer = io.filePointer;
    }
}
}
}