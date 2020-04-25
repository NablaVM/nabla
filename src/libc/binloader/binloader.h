#ifndef BINLOADER_H
#define BINLOADER_H

#include <stdio.h> 
#include "VmCommon.h"

typedef struct VM * NablaVirtualMachine;

//! \brief Loads a virtual machine with a binary
//! \returns Result code listed in vmrc.h prefixed with VM_LOAD_
int bin_load_vm(FILE* file, NablaVirtualMachine vm);

//! \brief Reset the bin loader
//! \post The bin loader will be able to be called again
void bin_reset();

#endif