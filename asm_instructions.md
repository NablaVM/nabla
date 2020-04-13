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

## Addresses

An address is comprised of 4 bytes, giving us 2^32 addresses.

## Comments

Everything after a stray ';' on a line will be treated as a comment

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

### In-place numerical values

For arithmatic operations a drop-in numerical value can be given to an instruction. This drop-in is the numerical value prefixed
with a '$'. These numerical drop-ins are unsigned and encoded directly into the instruction. The valid range for an in-place value
is the maximum number able to be stored by 2 bytes (2^16) 

### Directive references

Constant ints strings and doubles defined by their corresponding directive will yeild their address when placed within an instruction
by prefixing them with a '&'. Since they are addresses, they must be loaded into a register prior to performing an arithmatic operation.
This can be done using the 'ldw' command, which will drop their value into a register.

## Instructions
Abbreviations : 
| Abbreviation | Meaning                           |
|---           |---                                |
|      r       | register                          |
|      d       | double register                   |
|     sys      | system register                   |
|     *n       | in-place numerical value          |
|     *i       | constant int ref    (address)     |
|     *s       | constant string ref (address)     |
|     *d       | constant double ref (address)     |
|     sp       | stack pointer                     |
|     *sp      | stack pointer offset              |


## Artihmatic Instructions
| Instruction   | Arg1          | Arg2       | Arg3           | Description                        |
|---            |---            |---         |---             |---                                 |
|     add       |        r,  sp |    r , *n  |   r , *n       |  Add Arg2 and Arg3, Store in Arg1  |
|     mul       |        r      |    r , *n  |   r , *n       |  Mul Arg2 and Arg3, Store in Arg1  |
|     div       |        r      |    r , *n  |   r , *n       |  Div Arg3 by  Arg2, Store in Arg1  |
|     sub       |        r      |    r , *n  |   r , *n       |  Sub Arg3 from Arg3, Store in Arg1 |
|     addd      |        d      |    d       |   d            |  Add Arg2 and Arg3, Store in Arg1  |
|     muld      |        d      |    d       |   d            |  Mul Arg2 and Arg3, Store in Arg1  |
|     divd      |        d      |    d       |   d            |  Div Arg3 by  Arg2, Store in Arg1  |
|     subd      |        d      |    d       |   d            |  Sub Arg3 from Arg3, Store in Arg1 |

## Branch Instructions

| Instruction   | Arg1           | Arg2             | Arg3               | Description                      |
|---            |---             |---               |---                 |---                               |
|      bgt      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 > Arg2    |
|      bgte     |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 >= Arg2   |
|      blt      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 <= Arg2   |
|      blte     |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 <= Arg2   |
|      beq      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 == Arg2   |
|      bne      |  r, d          |   r, d           |   label            | Branch to Arg3 if Arg1 != Arg2   |

## Loading / Storing Instructions
|  Instruction     |  Arg1     |  Arg2               |  Description                                 |
|---               |---        |---                  |---                                           |
|      mov         |     r     |   r                 |  Move data from Arg2 into Arg1               |
|      movd        |     d     |   d                 |  Move data from Arg2 into Arg1               |
|      lda         |     r     |   *i, *s, *d, *sp   |  Load address of Arg2 into Arg1              |
|      stw         |     *sp   |   r                 |  Store Arg2 data at Arg1 (arg1=addr)         |
|      stwd        |     *sp   |   d                 |  Store Arg2 data at Arg1 (arg1=addr)         |
|      ldw         |     r     |   r, *i, *s, *sp    |  Load 4 bytes from Arg2 (address) into Arg1  |
|      ldwd        |     d     |   r, *d, *s         |  Load 8 bytes from Arg2 (address) into Arg1  |

*Note:* Moves destroy data in source. It will 0-out the data.
*Note:* ldw/ldwd assumes data from Arg2 represents an address, and will attempt to pull data from whatever address it assumes. We can leverage a dReg to load string data if we want to, but performing arithmatic operations on data representing a string will result in undefined behaviour (this applies for rRegs as well)
**Note:** The reason we can ldwd from a rReg is because we are ldwd-ing an address (4 bytes) that is stored in that reg.

## Jump / Call

| Instruction | Arg1  | Description                                   |
|---          |---    |---                                            |
| jmp         | label | Jump to label                                 |
| call        | label | Call label - return address stored in sys0    |
| ret         |       | Return to the address stored in sys0          |

## Exit

| Instruction | Description                                   |
|---          |---                                            |
| exit        | Quit execution of program                     |


### Constants and references

In-place constants must be prefixed with a '$' while referenced constants must be prefixed with a '&' 

For instance:

    add r0 $10 $10 ; Will add '10' to '10' and store in r0

Now, using a reference to a constant 

    .int EXAMPLE_INT 3265
    
    ldw r0 &EXAMPLE_INT     ; Load 3265 from its address into r0

    add r0 r0 $1            ; Add 1 to 3265, store in r0


**NOTE:** WE NEED TO LOOK AT HOW WE ARE GOING TO USE SP, AND WHAT ROLE IT WILL PLAY IN THE VM - This is not yet worked through
Perhaps instead of using sp at all, a 'reserve' keyword could be used and an 'address (@) ?' keyword could be used to specify a direct 
address location for ldw, etc


## Instruction Data

Each full instruction is 8 bytes. 

### Arithmatic operations

[Byte 1]
The first 6 bits represent the specific instruction (add / mul/ etc)
The remaining 2 bits of the first byte indicate what the remaining bytes represent.

The indication bits are as follows:
00 - Byte 3 will be a register, Byte 4 will be a register
01 - Byte 3 will be a register, Byte 4/5 will be a 2-byte integer
10 - Byte 3/4 will be a 2-byte integer, Byte 5 will be a register
11 - Byte 3/4 will be a 2-byte integer, Byte 5/6 will be a 2-byte integer

[Byte 2]
The second byte is the destination register

Unaccounted bytes will be unused, and marked as '1'

**Note:** Since drop-ins are not allowed on double-based arithmatic operations, the id bits listed above
and displayed below are not applicable. Instead, double-based arithmatic operations will default to the 
'00' case listed below.

Here is an example of the bit layout given an arithmatic operation. Note: All but ID here are filled
with '1' just for the sake of demonstration

    Case 00:
    INS    ID   REGISTER    REGISTER    REGISTER  [ ----------------- UNUSED -------------------]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 01:
    INS    ID   REGISTER    REGISTER    [ ---- INTEGER ---- ]   [ -------- UNUSED --------------]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 10:
    INS    ID   REGISTER    [ ---- INTEGER ---- ]   REGISTER    [ -------- UNUSED --------------]
    111111 10 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    Case 10:
    INS    ID   REGISTER    [ ---- INTEGER ---- ]   [ ---- INTEGER ---- ]   [ ---- UNUSED ---- ]
    111111 11 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111
    
### Branch operations

[Byte 1]
The first 6 bits represent the specific instruction (bge / blt / etc)
The following 2 bits will be set to 00, unused, but also represents that 2 registers are being used.

[Byte 2-3]
The following 2 bytes, are the registers that are used for the branch comparison.

[Byte 4-7]
The following 4 bytes will be the address to branch to.

[Byte 8]
The last byte is unused 

Here is an example of a bit layout for a branch operation

    INS    ID   REGISTER    REGISTER    [ --------------   ADDRESS  ----------------]   [ UNUSED ] 
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

### Load / Store operations

The first 6 bytes represent the specific instruction (mov / movd / etc)

Since not all load / store operations are the same their bit fields differ slightly by specific instruction.
For all instructions except ldw/ldwd the ID bits don't matter. 

**mov/movd**

    INS    ID   REGISTER    REGISTER    [ ----------------------- UNUSED ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**lda**

    INS    ID   REGISTER    [ ---------------   ADDRESS  ---------------]   [ ----- UNUSED ---- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**stw/stwd**

    INS    ID   [ ---------------   ADDRESS  ---------------]   REGISTER    [ ---- UNUSED ----- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**ldw/ldwd**

For ldw/ldwd the ID bits represent if the source is an address in a register, or if it is an address
stored within the instruction.

Id bits:
00 - Byte 3 is a register (presumably with an address in it)

    INS    ID   REGISTER    REGISTER    [ ----------------------- UNUSED ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

01 - Byte 3-6 is an address encoded into the instruction

    INS    ID   REGISTER    [ ---------------   ADDRESS  ---------------]   [ ---- UNUSED ----- ]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

### Jump / return operation

The jump operation is straight forward. The only data in the jump is the address to jump to.
Upon executing the jump instruction, the address immediatly after jump's address will be stored in
sys0 so that when a 'return' instruction is executed it can know where to go. 

    INS    ID   [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

Since return only reads the sys0 register for an address, its bit field is pretty straight forward. 

    INS    ID   REGISTER    [ ---------------   ADDRESS  ---------------]   [ ---- UNUSED ----- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

It is important to note, that since the jump instruction writes an address to sys0, care needs to be 
taken when executing sequential jumps as 


## The stack
