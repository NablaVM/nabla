#ifndef NABLA_VM_HOST_H
#define NABLA_VM_HOST_H

#include "vm.h"
#include <stdint.h>
#include <stdio.h>

//! \brief The Host Device struct
struct HOSTDevice;

//! \brief Create a new host device
//! \returns Device pointer
struct HOSTDevice * host_new();

//! \brief Indicate to the host device that processing is about to start
//! \param host The host to mark
//! \post The clock for host will be offset to mark vm processing time
void host_mark(struct HOSTDevice * host);

//! \brief Delete and clean-up an io device
//! \param host The host device to delete
//! \post  The host device memory will be freed
void host_delete(struct HOSTDevice * host);

//! \brief Process a host event with the host device
//! \param host The host device to process with
//! \param vm The vm that owns the host device
//! \post Errors are reported into the vm and need to be handled by a user
void host_process(struct HOSTDevice * host, struct VM * vm);


#endif