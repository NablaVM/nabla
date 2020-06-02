
<center> <h1> <b> ∇ Nabla </b> </h1> </center>

[![Build Status](https://travis-ci.com/NablaVM/nabla.svg?branch=master)](https://travis-ci.com/NablaVM/nabla)
![](https://img.shields.io/badge/License-MIT-blueviolet)
![](https://img.shields.io/badge/Built%20with-C%2B%2B-red)

## Development Updates: 

***1/ June/ 2020***

:scream: Compiler design is hard. I've been working with flex and bison to figure out a way I can get some HLL to get converted into Nabla ASM. I got _something_ working, but it isn't very nice. I don't want to create absolute garbage, so I bought a book. Once I do some research and learn the best way to go about constructing the HLL I will get going again. For now, what I have will be blocked off from the Nabla Application for compiling. The VM is still in working order, and still executes projects built with Solace. 


***15/ May/ 2020***

:rocket: Assembly and byte code have been fully fleshed out. 

:boom: There now exists an ASM syntax highlighting extension for VSCode (see below).

:space_invader: The Nabla HLL will now begin to be developed. 

### About 

The goal of Nabla is to establish a high level programming language on top of a register based virtual machine. The purpose is to explore the different aspects of language and virtual machine creation. 

### Language and Instruction set

As of now the high level language for Nabla has not yet been established. Work is still being done on the virtual machine. Once the project gets to a point that it can support all required aspects of computation, the high level language will begin to be developed. 
For now, Nabla is usable with the Nabla ASM code [described here](https://github.com/NablaVM/nabla/wiki/Nabla-ASM). Writing assembly isn't everyone's cup of tea, but Nabla ASM isn't all that bad. Just write the code, and feed it into the [Solace](https://github.com/NablaVM/solace) application to have a binary assembled that can be executed by Nabla.

### Byte Code

Nabla ASM represents 64-bit fixed-width machine instructions (byte code ...or.. 8-byte code?). 64-bit seems a bit heavy for a virtual machine that just runs a language, but since Nabla is a register-based virtual machine and not a stack-based faker it needed some detailed instructions. These detailed instructions allow us to express lots of information so we can do lots with each instruction.
To find out more about the Nabla byte code, check out the documentation [here](https://github.com/NablaVM/nabla/wiki/Nabla-Byte-Code). 


