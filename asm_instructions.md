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
|     dadd      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Add Arg2 and Arg3, Store in Arg1  |
|     dmul      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Mul Arg2 and Arg3, Store in Arg1  |
|     ddiv      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Div Arg3 by  Arg2, Store in Arg1  |
|     dsub      |        d      |    d , *d, *dn         |   d , *d, *dn       |  Sub Arg3 from Arg3, Store in Arg1 |

## Loading / Storing Instructions
|  Instruction     |  Arg1     |  Arg2               |  Description                               |
|---               |---        |---                  |---                                         |
|      mov         |     r     | r, *sp, *i, *n, sys |  Move data from Arg2 into Arg1             |
|      dmov        |     d     |   d,  *d, *nd       |  Move data from Arg2 into Arg1             |
|      ldw         |     r     |   *sp, *s           |  Load word from Arg2 into Arg1             |
|      dldw        |     d     |   *sp, d            |  Load double-word from Arg2 into Arg1      |
|      stw         |     *sp   |   r                 |  Store Arg2 at Arg1                        |
|      dstw        |     *sp   |   d                 |  Store Arg2 at Arg1                        |
|      ldc         |     r     |   *n, *i            |  Load constant from Arg2 into Arg1         |
|      dldc        |     d     |   *d                |  Load double-constant from Arg2 into Arg1  |

## Branch Instructions

| Instruction   | Arg1           | Arg2             | Arg3               | Description                      |
|---            |---             |---               |---                 |---                               |
|      bgt      |  r, d, *i, *d  |   r, d, *i, *d   |   label            | Branch to Arg3 if Arg1 > Arg2    |
|      bgte     |  r, d, *i, *d  |   r, d, *i, *d   |   label            | Branch to Arg3 if Arg1 >= Arg2   |
|      blt      |  r, d, *i, *d  |   r, d, *i, *d   |   label            | Branch to Arg3 if Arg1 <= Arg2   |
|      blte     |  r, d, *i, *d  |   r, d, *i, *d   |   label            | Branch to Arg3 if Arg1 <= Arg2   |
|      beq      |  r, d, *i, *d  |   r, d, *i, *d   |   label            | Branch to Arg3 if Arg1 == Arg2   |
|      bne      |  r, d, *i, *d  |   r, d, *i, *d   |   label            | Branch to Arg3 if Arg1 != Arg2   |

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

To use stack pointer as a destination, simply use 'sp'.
To use a stack pointer offset, prefix with an in-place constant i.e : $10(sp)
A stack pointer can be offset with a regular constant as well i.e   : &EXAMPLE_INT(sp)
