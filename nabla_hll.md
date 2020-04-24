# High Level Language

**NOTE**: None of this is done, or even started. It is idea-ing. 


The nabla high level language can be compiled to bytecode in bulk or interpreted. 

Targets compiled to bytecode can be loaded into a vm using libc/binloader and then executed using vm_run()

Targets interpreted will convert each line into bytecode and then the libcpp/interpreter will manage injecting the bytecode into
the vm directly, and handling the calls to cycle the vm to complete execution.

The aim will be to have the compiler and interpreter leverage the same objects / methods for deconstructing the HLL into bytecode, but
then go their seperate ways. The compiler can handle program-wide optimization, then dump the bytecode to a file that can be loaded into a vm
using libc/binloader. The interpreter will take the bytecode for each line and manage VM state and execution of bytecode.
