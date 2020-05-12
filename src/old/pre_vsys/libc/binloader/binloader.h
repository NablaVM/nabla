#ifndef BINLOADER_H
#define BINLOADER_H

#include <stdio.h> 
#include "vm.h"

/*
    Potential return values for vm_load_file
*/

#define BIN_LOAD_ERROR_NULL_VM                  -10
#define BIN_LOAD_ERROR_FILE_OPEN                -11
#define BIN_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS -12
#define BIN_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION  -13
#define BIN_LOAD_ERROR_UNHANDLED_INSTRUCTION    -14
#define BIN_LOAD_ERROR_ALREADY_LOADED           -15
#define BIN_LOAD_ERROR_EOB_NOT_FOUND            -16

typedef struct VM * NablaVirtualMachine;

//! \brief Loads a virtual machine with a binary
//! \returns Result code listed in vmrc.h prefixed with VM_LOAD_
int bin_load_vm(FILE* file, NablaVirtualMachine vm);

//! \brief Reset the bin loader
//! \post The bin loader will be able to be called again
void bin_reset();

#endif