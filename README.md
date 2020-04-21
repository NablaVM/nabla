
<center> <h1> <b> ∇ </b> </h1> </center>
<center> <h1> <b> Nabla </b> </h1> </center>


### About
This is the Nabla virtual machine project. 

The virtual machine is set to run the instructions found in asm_instructions.md. Currently, in order to execute code on the vm, the instructions must be written in the Nabla assembly code. The instruction set can be found in asm_instructions.md, and examples can be found in solace/solace_code. 

    **Note:** The code in solace_code is being used during the development of Nabla and issues may arise. 
    Once the base instruction set is _fully_ tested there will be an examples directory made of code that should always be working. 

### Compiling

The current compiler for Nabla is **solace**. Once built, solace takes whatever file contains the instructions and dumps out a binary called 'solace.out'. This binary can be loaded into the vm and executed. 

### Goal

The goal of the Nabla project is so create a robust register-based virtual machine that quickly and correctly executes a 64-bit instruction set. The hope is to allow for reading compiled byte code, or to use directly with a higher level language (coming soon!) that can be interpreted and executed on the VM. 

### Why

Because virtual machines are interesting. 