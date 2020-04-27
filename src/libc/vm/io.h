#ifndef NABLA_VM_IO_H
#define NABLA_VM_IO_H

#include "vm.h"

#define NABLA_VM_IO_INPUT_SETTINGS_MAX_IN   1024

//!\brief Reads in a max of 1024 bytes. Number of bytes read-in will be reported 
int io_stdin(struct VM * vm);

#endif