#ifndef NABLA_VM_SETTINGS_H
#define NABLA_VM_SETTINGS_H

#define NABLA_SETTINGS_LOCAL_STACK_SIZE     125000      // Each frame of the stack is 8 bytes, so this should be 1mb

#define NABLA_SETTINGS_GLOBAL_STACK_SIZE    62500000    // 500 mb
    

#define NABLA_SETTINGS_MAX_FUNCTIONS        500         // Each can have a single local stack, so (this * local_stack_size) in total

#define NABLA_SETTINGS_CALL_STACK_SIZE      NABLA_SETTINGS_MAX_FUNCTIONS * 3  // Each call requires 3 instructions, so allow one call for each function max


#endif