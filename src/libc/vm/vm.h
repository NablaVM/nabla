#ifndef NABLA_VM_H
#define NABLA_VM_H

#include "VmCommon.h"


//! \brief Creates a new vm
//! \returns Vm pointer
struct VM* vm_new();

//! \brief Delete the vm
//! \post The VM data will be freed, and vm_init can be ran on a new vm
void vm_delete(struct VM* vm);

/* 
    ----------------------------------------------

        Method for Binary Execution (post-binloader) 

    ----------------------------------------------
*/

//! \brief Runs a vm until end of loaded instruction set
//! \returns Result code listed in libc/VmCommon.h
int vm_run(struct VM* vm);

/* 
    ----------------------------------------------

        Methods for Interpreter 

    ----------------------------------------------
*/

//! \brief Initialize a vm. Only needs to be done if vm_step is going to be called.
//!        as vm_run will handle this call for you.
//!        vm_init can only be called once per application execution, otherwise it will
//!        ignore your request and return an error
//! \post  vm_step can be safely called
//! \returns Result code listed in libc/VmCommon.h
//!
int vm_init(struct VM* vm);

//! \brief Steps a vm by 'n' instruction cycles unless end of loaded instruction set is found
//! \pre   vm_init must have been called
//! \returns Result code listed in libc/VmCommon.h
int vm_step(struct VM* vm, uint64_t n);

#endif