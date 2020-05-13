#include "VSysExternalIO.hpp"

extern "C"
{
    #include "util.h"
    #include <assert.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
}

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
        int      gsByteIndex;
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


    /*
    
            New IO Mechanism Adapter
    
    */

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    IO::IO() : state(static_cast<int>(IODeviceState_Close)), filePointer(nullptr), byteIndex(0)
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
            this->filePointer, 
            this->byteIndex 
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
               // process_io_out(io, vm, STDOUT_FILENO);
                break;
            } 

            case NABLA_IO_DEVICE_STDERR  :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
               printf("io_stderr\n");
#endif
          //      process_io_out(io, vm, STDERR_FILENO);
                break;
            } 

            case NABLA_IO_DEVICE_DISKIN  :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("process_io_disk_in\n");
#endif
            //    process_io_disk_in(io, vm);
                break;
            } 

            case NABLA_IO_DEVICE_DISCKOUT:
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("process_io_disk_out\n");
#endif
                //process_io_disk_out(io, vm);
                break;
            } 

            case NABLA_IO_DEVICE_CLOSE   :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("process_close_io\n");
#endif
                //process_close_io(io, vm);
                break;
            } 

            case NABLA_IO_DEVICE_REPORT :
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("io_report\n");
#endif
                //process_report(io,vm);
                break;
            }

            default:
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("io_process: Unknown target\n");
#endif
                process_close_io(&io, &vm);
                return;
        }



    }
}
}
}