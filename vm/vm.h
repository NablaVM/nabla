#ifndef NABLA_VM_H
#define NABLA_VM_H

#include <stdio.h> 

//! \brief The nabla virtual machine
typedef struct VM * NablaVirtualMachine;

//! \brief Creates a new vm
//! \returns Vm struct
NablaVirtualMachine vm_new();

int vm_load_file(FILE* file, NablaVirtualMachine vm);

#endif