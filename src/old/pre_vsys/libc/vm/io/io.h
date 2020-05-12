#ifndef NABLA_VM_IO_H
#define NABLA_VM_IO_H

#include "vm.h"
#include <stdint.h>
#include <stdio.h>

//! \brief The IO Device struct
struct IODevice;

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
void io_process(struct IODevice * io, struct VM * vm);


#endif