#ifndef NABLA_VM_IO_H
#define NABLA_VM_IO_H

#include "vm.h"
#include <stdint.h>
#include <stdio.h>

#define NABLA_DEVICE_ADDRESS_IO 0x0A

#define NABLA_VM_IO_INPUT_SETTINGS_MAX_IN   1024

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


#endif