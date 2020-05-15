#ifndef NABLA_VM_NET_H
#define NABLA_VM_NET_H

#include "vm.h"

//! \brief The NET Device struct
struct NETDevice;

//! \brief Create a new net device
//! \returns Device pointer
struct NETDevice * net_new();

//! \brief Delete and clean-up a net device
//! \post  The net work device will have all allocated connection objects
//!        closed and cleaned. The device will be freed
void net_delete(struct NETDevice * net);

//! \brief Process a network instruction
//! \param net The net device
//! \param vm The vm that owns the net device
//! \post Errors are reported into the vm and need to be handled by a user
void net_process(struct NETDevice * net, struct VM * vm);

#endif