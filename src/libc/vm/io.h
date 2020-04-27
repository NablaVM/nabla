#ifndef NABLA_VM_IO_H
#define NABLA_VM_IO_H

#include "vm.h"

#define NABLA_VM_IO_INPUT_SETTINGS_MAX_IN   1024

//!\brief Reads in a max of 1024 bytes. Number of bytes read-in will be reported 
//!       Triggered by data value '1' in register 10. 
//!\retval 0 Okay, bad otherwise (error handling not yet set)
//!\post  Number of bytes read-in will be in register 11, Number of stack frames 
//!       produced will be in register 12
//!       All input data from this call will be pushed into the global stack. Since stacks
//!       are FIFO, this method ensures the placement in the stack is such-that popping data 
//!       from gs sequentially will produce the in-order string of characters that was read in
int io_stdin(struct VM * vm);

//!\brief Writes out a uint64_t byte by byte starting with the MSB. 
//!       Triggered by data value '2' in register 10
//!       The uint64_t value it writes is whatever is in register 11
int io_stdout(struct VM *vm);

//!\brief Writes out a uint64_t byte by byte starting with the MSB. 
//!       Triggered by data value '3' in register 10
//!       The uint64_t value it writes is whatever is in register 11
int io_stderr(struct VM *vm);

#endif