#ifndef NABLA_VM_RETURN_CODES_H
#define NABLA_VM_RETURN_CODES_H

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

#endif