#ifndef NABLA_VM_IO_H
#define NABLA_VM_IO_H

#include "vm.h"
#include <stdint.h>
#include <stdio.h>

#define NABLA_DEVICE_ADDRESS_IO 0x0A

#define NABLA_VM_IO_INPUT_SETTINGS_MAX_IN   1024

/*
    The io device will be triggered to open in a particular mode, and depending on that
    mode it will need certain data to set up. Once the device is open, it needs to be 
    'ticked' or instructed to continue operation until it is told to cease whatever its doing.
    In order to use the IO device a 'driver' of sorts will need to be written.

    Example : 

        Move val into r10 that specified mode.
            if mode is stdin
                setup std in, get number of bytes specified in r11. Place in gs
                return without unsetting r10. 

                At this point the driver will have to tell the device either to stop by 
                instructing a close, or to indicate how many more values to read by placeing 
                val num in r11 and allowing the triggering of r10 again

            if mode is stdout
                byte(s) to write should be placed in r11. And the stdout trigger placed into r10

                The device will open stdout and write contents of r11.

                This process will continue until close instruction given, and r10 cleared

            Same goes for disk operations, with the exception they may have further instructions
            to move the fp . Idk yet

*/


// Different target types for the IO Device
enum IODeviceTarget
{
    IODeviceTarget_Stdin,    // Standard input
    IODeviceTarget_Stdout,   // Standard output
    IODeviceTarget_Stderr,   // Standard error
    IODeviceTarget_DiskIn,   // Disk input
    IODeviceTarget_DiskOut,  // Disk output
    IODeviceTarget_Close,    // Close instruction
    IODeviceTarget_None      // No instruction 
};

// The input / output stream handler
struct IODevice
{
    enum    IODeviceTarget target;  // Target specified 
    uint8_t isDeviceActive;         // Check if the device is active
    FILE *  filePointer;            // File pointer for device
};

//! \brief Create a new io device
//! \returns Device pointer
struct IODevice * io_new();

//! \brief Delete and clean-up an io device
//! \post  The io memory will be freed
void io_delete(struct IODevice * io);

//! \brief Process an io event with the io device
//! \param io The io device to process with
//! \param vm The vm that owns the io device
//! \post Errors are reported into the vm and need to be handled by a user
//!       unless they are fatal, then the whole stack of cards will fall
void io_process(struct IODevice * io, struct VM * vm);




// ------------------------------ OLD IO - Left until iodevice is implemented ----------


//!\brief Reads in a max of 1024 bytes. Number of bytes read-in will be reported 
//!       Triggered by data value '1' in register 10. 
//!\retval 0 Okay, bad otherwise (error handling not yet set)
//!\post  Number of bytes read-in will be in register 11, Number of stack frames 
//!       produced will be in register 12
//!       All input data from this call will be pushed into the global stack. Since stacks
//!       are FIFO, this method ensures the placement in the stack is such-that popping data 
//!       from gs sequentially will produce the in-order string of characters that was read in
int io_stdin(struct VM * vm);

//!\brief Writes out a uint64_t byte by byte starting with the MSB. 
//!       Triggered by data value '2' in register 10
//!       The uint64_t value it writes is whatever is in register 11
int io_stdout(struct VM *vm);

//!\brief Writes out a uint64_t byte by byte starting with the MSB. 
//!       Triggered by data value '3' in register 10
//!       The uint64_t value it writes is whatever is in register 11
int io_stderr(struct VM *vm);

#endif