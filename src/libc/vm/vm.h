#ifndef NABLA_VM_H
#define NABLA_VM_H

#include "stack.h"
#include <stdint.h>

// This is set by CMake now, but being left in case someone wonders where it came from
// and naturally decides to look here
#if 0
    #define NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
#endif

#define VM_INIT_ERROR_ALREADY_INITIALIZED      -30

/*
    Potential return values for vm_run
*/
#define VM_RUN_ERROR_VM_ALREADY_RUNNING        -51
#define VM_RUN_ERROR_INSTRUCTION_NOT_FOUND     -100
#define VM_RUN_ERROR_UNKNOWN_INSTRUCTION       -101
#define VM_RUN_ERROR_VM_NOT_INITIALIZED        -102

/*
    Nabla VM settings
*/
#define VM_SETTINGS_BYTES_PER_INS            8           // WARNING : DO _NOT_ change this - Placed here for consistency and reference only
#define VM_SETTINGS_LOCAL_STACK_SIZE         125000      // Each frame of the stack is 8 bytes, so this should be 1mb
#define VM_SETTINGS_GLOBAL_STACK_SIZE        62500000    // 500 mb
#define VM_SETTINGS_MAX_FUNCTIONS            500         // Each can have a single local stack, so (this * local_stack_size) in total
#define VM_SETTINGS_CALL_STACK_SIZE         VM_SETTINGS_MAX_FUNCTIONS * 3  // Each call requires 3 instructions, so allow one call for each function max
#define VM_SETTINGS_MAX_IN_PER_FUCNTION      100000 // Each instruction is 8 bytes, so this could be X *VM_SETTINGS_MAX_FUNCTIONS * 8 bytes (currently 400mb)

/*
    VM Structure - We need this public so binloader and other things can manipulate the specifics of the vm
*/

//  A function of instructions
//
struct FUNC
{
    uint64_t ip;             // Instruction pointer
    NablaStack instructions; // Instructions
    NablaStack localStack;   // Local stack for storage
};

//  The virtual machine
//
struct VM
{
    uint8_t  id;             // Specific vm id
    uint64_t fp;             // Function pointer
    uint64_t entryAddress;   // Entry function address listed in binary
    uint64_t numberOfFunctions;
    int64_t  registers[16];  // VM Registers
    NablaStack globalStack;  // Shared 'global' stack
    NablaStack callStack;    // Call stack
    struct FUNC * functions; 

   // struct IODevice * io;
};

/*
    VM usage methods
*/

/* 
    ----------------------------------------------

        Method for creating and destroying a vm

    ----------------------------------------------
*/

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