
<center> <h1> <b> ∇ Nabla </b> </h1> </center>

[![Build Status](https://travis-ci.com/NablaVM/nabla.svg?branch=master)](https://travis-ci.com/NablaVM/nabla)
![](https://img.shields.io/badge/License-MIT-blueviolet)
![](https://img.shields.io/badge/Built%20with-C%2B%2B-red)

### About 

This is an implementation of the Nabla Virtual Machine (NVM). The purpose of the NVM is to explore VM design and language construction. 

### Language and Instruction set

To use the NVM you can write some [ASM](https://github.com/NablaVM/nabla/wiki/Nabla-ASM) and assemble it with the [Solace](https://github.com/NablaVM/solace) application, or if you're feeling more adventurous, you can use the [Del Compiler](https://github.com/NablaVM/del) to write in a high level language and have it compiled to NVM byte code. Right now the Del Compiler is under heavy development and isn't too extensive. Also, the Del Compiler doesn't have any documentation yet. Once it comes along that information will be posted here.

### Byte Code - Some information

Nabla ASM represents 64-bit fixed-width machine instructions (byte code ...or.. 8-byte code?). 64-bit seems a bit heavy for a virtual machine that just runs a language, but since Nabla is a register-based virtual machine and not a stack-based faker it needed some detailed instructions. These detailed instructions allow us to express lots of information so we can do lots with each instruction.
To find out more about the Nabla byte code, check out the documentation [here](https://github.com/NablaVM/nabla/wiki/Nabla-Byte-Code). 


