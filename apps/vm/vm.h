#ifndef NABLA_VM_H
#define NABLA_VM_H

#include "VmCommon.h"

//! \brief Creates a new vm
//! \returns Vm pointer
struct VM* vm_new();

//! \brief Creates a new vm
//! \returns Result code listed in vmrc.h prefixed with VM_RUN_
int vm_run(struct VM* vm);

#endif