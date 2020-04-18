#ifndef NABLA_VM_H
#define NABLA_VM_H

#include <stdio.h> 

// Set if to 0 to disable debug
#if 1
    #define NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
#endif

//! \brief The nabla virtual machine
typedef struct VM * NablaVirtualMachine;

//! \brief Creates a new vm
//! \returns Vm pointer
NablaVirtualMachine vm_new();

//! \brief Loads a virtual machine with a binary
//! \returns Result code listed in vmrc.h prefixed with VM_LOAD_
int vm_load_file(FILE* file, NablaVirtualMachine vm);

//! \brief Creates a new vm
//! \returns Result code listed in vmrc.h prefixed with VM_RUN_
int vm_run(NablaVirtualMachine vm);

#endif