
<center> <h1> <b> ∇ Nabla </b> </h1> </center>

[![Build Status](https://travis-ci.com/NablaVM/nabla.svg?branch=master)](https://travis-ci.com/NablaVM/nabla)
![](https://img.shields.io/badge/License-MIT-blueviolet)
![](https://img.shields.io/badge/Built%20with-C%2B%2B-red)

## Development Updates: 

***15/ May/ 2020***

:rocket: Assembly and byte code have been fully fleshed out. [Check out the examples](https://github.com/bosley/nabla/tree/master/examples/asm)! 

:boom: There now exists an ASM syntax highlighting extension for VSCode (see below).

:space_invader: The Nabla HLL will now begin to be developed. 

### About 

The goal of Nabla is to establish a high level programming language on top of a register based virtual machine. The purpose is to explore the different aspects of language and virtual machine creation. 

### Language and Instruction set

As of now the high level language for Nabla has not yet been established. Work is still being done on the virtual machine. Once the project gets to a point that it can support all required aspects of computation, the high level language will begin to be developed. 
For now, Nabla is usable with the Nabla ASM code [described here](https://github.com/bosley/nabla/wiki/Nabla-ASM). Writing assembly isn't everyone's cup of tea, but Nabla ASM isn't all that bad. Just write the code, and feed it into the [Solace](https://github.com/bosley/nabla/tree/master/src/apps/solace) application to have a binary assembled that can be executed by Nabla.

### Byte Code

Nabla ASM represents 64-bit fixed-width machine instructions (byte code ...or.. 8-byte code?). 64-bit seems a bit heavy for a virtual machine that just runs a language, but since Nabla is a register-based virtual machine and not a stack-based faker it needed some detailed instructions. These detailed instructions allow us to express lots of information so we can do lots with each instruction.
To find out more about the Nabla byte code, check out the documentation [here](https://github.com/bosley/nabla/wiki/Nabla-Byte-Code). 

### Applications

Inside the src of this repo there exists an 'apps' directory. In this directory 2 apps can be found. Solace, and Nabla. 

**Solace** : This is the assembler. It takes hand-crafted assembly instructions and generates a binary file that can be executed by nabla.

**Nabla** : Obviously this is where the cake is. Nabla is the application that takes in and executes byte code. It currently works only with compiled binary files produced by Solace, but as the high level language is developed, it will also be responsible for dropping a user into an interpreter. _wow!_ This means that code will be able to be compiled to byte code AND interpreted! Wild. The particulars of how the interpreter will work, and what the syntax for the high level language hasn't been thought out yet. Once they start to get created documents in the Nabla wiki will be made available. 

### Extensions

Under vscode_extensions there is an extension for highlighting the Nabla ASM syntax within VS Code. Copy it to your vscode extension directory to make things look better while slamming out some ASM. Eventually, when the ASM instructions are locked by version, the extension will most likely be published so it can be easily installed.
