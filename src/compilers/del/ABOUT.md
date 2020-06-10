# About

This is the compiler setup for the ***Del*** language. 



Life of a statement :

    Flex/Bison -> Driver -> Analyzer -> Intermediate -> Codegen -> ASM -> Assembler -> Bytecode
                                |        |  |
                            Symbol Table /  |
                                |           |
                              Memory -------/

