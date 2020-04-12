# This will be expanded as things move along, 

### Registers

There will be 2 types of registers

## 4-Byte Registers

Prefixed with 'r' there are 16 registers [ r0, r15 ]
These registers are used to store integers and strings, Each register is 4 bytes,

[64 bytes in total]

## 8-Byte registers

Prefixed with 'd' there are 8 double registers [d0, d7]
These registers are used to store double precision numbers and to perform operations on,
Each register is 8 bytes,

[64 bytes in total]

## Comments

Everything after a stray '#' on a line will be treated as a comment

## Read-only system registers

Prefixed with 'sys' there are 2 system registers [sys0, sys1]
These registers are read-only to the application. They are used by the system for storing return addresses
and performing system functions.
Each register is 4 bytes.

[8 bytes in total]

### Pseudo Ops
|  Directive 	|   Argument	|   Description                                 |
|---	        |---	        |---	                                        |
|   .file	    |  "file name"  |   emit filename FILE_LOCAL to symbol table	|
|   .init	    |  entry_label 	|   emit entry_label APP_ENTRY to symbol table 	|
|   .string	    |  "string"     |   emit "string" ID to string table	        |
|   .int	    |     42        |   emit 42 ID to integer table	                |
|   .double	    |   3.14 	    |   emit 3,14 ID to double table            	|
|   .include	|  "file.asm" 	|   add "file,asm" to source                   	|

## Instructions
Abbreviations : 
| Abbreviation | Meaning                 |
|---           |---                      |
|      r       | register                |
|      d       | double register         |
|     sys      | system register         |
|     *n       | numerical value         |
|     *i       | constant int ref        |
|     *s       | constant string ref     |
|     *d       | constant double ref     |
|     *dn      | double numerical value  |
|     sp       | stack pointer           |
|     *sp      | stack pointer offset    |


## Artihmatic Instructions
| Instruction   | Arg1          | Arg2                   | Arg3                | Description                        |
|---            |---            |---                     |---                  |---                                 |
|     add       |        r,  sp |    r , *n, *i , *sp    |   r , *n, *i   *sp  |  Add Arg2 and Arg3, Store in Arg1  |
|     mul       |        r      |    r , *n, *i , *sp    |   r , *n, *i , *sp  |  Mul Arg2 and Arg3, Store in Arg1  |
|     div       |        r      |    r , *n, *i , *sp    |   r , *n, *i , *sp  |  Div Arg3 by  Arg2, Store in Arg1  |
|     sub       |        r      |    r , *n, *i , *sp    |   r , *n, *i , *sp  |  Sub Arg3 from Arg3, Store in Arg1 |
|     addd      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Add Arg2 and Arg3, Store in Arg1  |
|     muld      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Mul Arg2 and Arg3, Store in Arg1  |
|     divd      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Div Arg3 by  Arg2, Store in Arg1  |
|     subd      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Sub Arg3 from Arg3, Store in Arg1 |

## Branch Instructions

| Instruction   | Arg1           | Arg2             | Arg3               | Description                      |
|---            |---             |---               |---                 |---                               |
|      bgt      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 > Arg2    |
|      bgte     |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 >= Arg2   |
|      blt      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 <= Arg2   |
|      blte     |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 <= Arg2   |
|      beq      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 == Arg2   |
|      bne      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 != Arg2   |


    THESE STILL NEED TO BE WORKED OUT - AND THE SOLACE PARSER WILL NEED TO BE UPDATED

## Loading / Storing Instructions
|  Instruction     |  Arg1     |  Arg2               |  Description                               |
|---               |---        |---                  |---                                         |
|      mov         |     r     |   r, *i, *n, sys    |  Move data from Arg2 into Arg1             |
|      movd        |     d     |   d, *d, *nd        |  Move data from Arg2 into Arg1             |
|      lda         |     r     |   *sp, *s, *i *d    |  Load address from Arg2 into Arg1          |
|      stw         |     *sp   |   r                 |  Store Arg2 at Arg1                        |
|      stwd        |     *sp   |   d                 |  Store Arg2 at Arg1                        |
|      ldc         |     r     |   *n, *i            |  Load constant from Arg2 into Arg1         |
|      ldcd        |     d     |   *d                |  Load double-constant from Arg2 into Arg1  |

## Jump

| Instruction | Arg1  | Description                                   |
|---          |---    |---                                            |
| jmp         | label | Jump to label - return address stored in sys0 |
| ret         |       | Return to the address stored in sys0          |

### Constants and references

In-place constants must be prefixed with a '$' while referenced constants must be prefixed with a '&' 

For instance:

    add r0 $10 $10 ; Will add '10' to '10' and store in r0

Now, using a reference to a constant 

    .int EXAMPLE_INT 3265
    
    add r0 $10 &EXAMPLE_INT ; adds '3265' to '10' and stores in r0


## Using stack pointer

To use a stack pointer offset, prefix with an in-place constant i.e : $10(sp)
A stack pointer can be offset with a regular constant as well i.e   : &EXAMPLE_INT(sp)


## Instruction Data
Each full instruction is 8 bytes. 

### Arithmatic operations

The first 6 bits represent the specific instruction (add / mul/ etc)
The remaining 2 bits of the first byte indicate what the remaining bytes represent.

The indication bits are as follows:
00 - Next byte is a register, the byte following is also a register
01 - Next byte is a register, the following three are an address
10 - Next three bytes are an address, with the following byte being a register
11 - Next three bytes are an address, the following three are also an address

This means that addressing has a maximum of 2^24 addresses

Here is an example of the bit layout given an arithmatic operation. Note: All but ID here are filled
with '1' just for the sake of demonstration

    Case 00:
    INS    ID   REGISTER    REGISTER    [ ------------------ UNUSED ----------------------------]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 01:
    INS    ID   REGISTER    [ -------   ADDRESS  -----------]   [ ---------- UNUSED ------------]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 10:
    INS    ID   [ --------   ADDRESS  ----------]   REGISTER    [ ---------- UNUSED ------------]
    111111 10 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 11:
    INS    ID   [ --------   ADDRESS   ----------]  [ --------   ADDRESS   ----------]  [UNUSED]
    111111 11 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

### Branch operations

Since addresses are 3 bytes, and instructions are 8, branches are limited to working with registers.
This way, we can stay within the 8 byte instruction size (with the draw back of having extra instruction to load registers)

The first 6 bytes represent the specific instruction (bge / blt / etc)
The following 2 bits will be set to 00, unused, but also represents that 2 registers are being used.
The following 2 bytes, are the registers that are used for the branch comparison.
The following 3 bytes will be the address to branch to.

Here is an example of a bit layout for a branch operation

    INS    ID   REGISTER    REGISTER    [ --------   ADDRESS  ----------]   [ ----- UNUSED ---- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

### Load / Store operations

The first 6 bytes represent the specific instruction (mov / movd / etc)

Since not all load / store operations are the same their bit fields differ slightly by specific instruction.

**stw/stwd**

    INS    ID   REGISTER    REGISTER    [ --------   ADDRESS  ----------]   [ ----- UNUSED ---- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111


