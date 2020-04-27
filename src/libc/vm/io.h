#ifndef NABLA_VM_IO_H
#define NABLA_VM_IO_H

#include "vm.h"

#define NABLA_VM_IO_INPUT_SETTINGS_MAX_IN   1024

#define NABLA_VM_IO_INPUT_READ_ONE            10
#define NABLA_VM_IO_INPUT_READ_N              11
#define NABLA_VM_IO_INPUT_READ_INT            12
#define NABLA_VM_IO_INPUT_READ_DOUBLE         13

#define NABLA_VM_IO_OUPUT_SIGNED_INT         200
#define NABLA_VM_IO_OUPUT_UNSIGNED_DEC       201
#define NABLA_VM_IO_OUPUT_UNSIGNED_HEX       202
#define NABLA_VM_IO_OUPUT_FLOATIN_POINT      203
#define NABLA_VM_IO_OUPUT_MIN_FLOATING_POINT 204
#define NABLA_VM_IO_OUPUT_CHAR               205
#define NABLA_VM_IO_OUPUT_STRING             206




/*
    The idea with IO is that if the io device flag is raised in the vm, then io is triggered. 
    Depending on what is in the register for IO it will either read in, or write out. 

    Writing out will be easier, as it can write out whatever is in a particular register, 
    while reading in is a little bit more difficult.

        Do we limit the input ? do we allow upper-limited? Either way, it will read in 
        and dump into the global stack. Placing the number of stack-frames read in into a
        register so the software can then go through the stack and do whatever to it.

*/

int io_invoke(struct VM * vm);

#endif