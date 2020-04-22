#ifndef NABLA_VM_COMMON_H
#define NABLA_VM_COMMON_H

#include "stack.h"
#include <stdint.h>

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
    int64_t  registers[16];  // VM Registers
    NablaStack globalStack;  // Shared 'global' stack
    NablaStack callStack;    // Call stack
    struct FUNC * functions; 
};

// Set if to 0 to disable debug
#if 1
    #define NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
#endif

/*
    Potential return values for vm_load_file
*/
#define VM_LOAD_ERROR_NULL_VM                  -10
#define VM_LOAD_ERROR_FILE_OPEN                -11
#define VM_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS -12
#define VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION  -13
#define VM_LOAD_ERROR_UNHANDLED_INSTRUCTION    -14
#define VM_LOAD_ERROR_ALREADY_LOADED           -15
#define VM_LOAD_ERROR_EOB_NOT_FOUND            -16

/*
    Potential return values for vm_run
*/
#define VM_RUN_ERROR_VM_NOT_LOADED             -50
#define VM_RUN_ERROR_VM_ALREADY_RUNNING        -51
#define VM_RUN_ERROR_INSTRUCTION_NOT_FOUND     -100
#define VM_RUN_ERROR_UNKNOWN_INSTRUCTION       -101

/*
    Nabla VM settings
*/
#define NABLA_SETTINGS_BYTES_PER_INS            8           // WARNING : DO _NOT_ change this - Placed here for consistency and reference only
#define NABLA_SETTINGS_LOCAL_STACK_SIZE         125000      // Each frame of the stack is 8 bytes, so this should be 1mb
#define NABLA_SETTINGS_GLOBAL_STACK_SIZE        62500000    // 500 mb
#define NABLA_SETTINGS_MAX_FUNCTIONS            500         // Each can have a single local stack, so (this * local_stack_size) in total
#define NABLA_SETTINGS_CALL_STACK_SIZE          NABLA_SETTINGS_MAX_FUNCTIONS * 3  // Each call requires 3 instructions, so allow one call for each function max
#define NABLA_SETTINGS_MAX_IN_PER_FUCNTION      100000 // Each instruction is 8 bytes, so this could be X * NABLA_SETTINGS_MAX_FUNCTIONS * 8 bytes (currently 400mb)

#endif