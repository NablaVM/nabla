# Nabla Byte Code

## Registers

Prefixed with 'r' there are 16 8-byte registers [ r0, r15 ]
These registers are used to read/write any data to/from.
The addresses for the registers are [0b, 15b]

[128 bytes overhead] * Number of execution contexts (more on that later)

## Comments

Everything after a stray ';' on a line will be treated as a comment

## Functions

Functions begin with a '<' followed by a name and a ':'. Functions are each given their own operational stack. This allows for the separation of functional memory. Standard function's stack will be emptied upon the return from that function. Every program function can access the following: its own stack, the global stack, and the system registers.

To access a function, a 'call' must occur, you can not jump to a function. Further, you can not jump to a label in another function. Jumping is localized to the function that is currently being operated in.

### Pseudo Ops
|  Directive 	|   Argument	|   Description                                 |
|---	        |---	        |---	                                        |
|   .file	    |  "file name"  |   To help debugging, not required         	|
|   .init	    |  entry_label 	|   emit entry_label APP_ENTRY to symbol table 	|
|   .string	    |  "string"     |   emit "string" to global stack    	        |
|   .int8	    |     42        |   emit intN to global stack	                |
|   .int16	    |     42        |   emit intN to global stack	                |
|   .int32	    |     42        |   emit intN to global stack	                |
|   .int64	    |     42        |   emit intN to global stack	                |
|   .double	    |   3.14 	    |   emit 3.14 to global stack                	|

### In-place numerical values

For arithmetic operations a drop-in numerical value can be given to an instruction. This drop-in is the numerical value prefixed
with a '$'. These numerical drop-ins are unsigned and encoded directly into the instruction. The valid range for an in-place value
is the maximum number able to be stored by 2 bytes (2^16) 

### Constants

Upon startup all constants listed will be loaded directly into the global stack. These constants will be loaded in the order by which they 
are defined in the file. Ints and doubles that are loaded by const are checked to ensure that they are valid range. 
Strings are not limited in size. Ints and doubles will need to be manually checked for sign, as once they're in the VM memory, they are just 
bits and bytes. 

**Note**: The size of the global stack is dependant on the constants. It is up to the user to know how to index into the constants. 

## Instructions
Abbreviations : 
| Abbreviation | Meaning                                |
|---           |---                                     |
|      r       | register                               |
|     *n       | in-place numerical value               |
|     sp       | stack pointer   (ls, gs)               |
|     *sp      | stack pointer offset  ($N(ls), $N(gs)) or rN(ls), rN(gs) for register-based offset |

**Note**: Stack offsets refer to 'byte' offsets. so $7(gs) is to refer to the 7th byte stored within the global stack

## Misc Instructions
| Instruction     | Arg1      | Arg2          | Arg3         | Description                                  |
|---              |---        |---            |---           |---                                           |
|     nop         |    NA     |    NA         |   NA         |  No Operation                                |
|     size        |    sp     |    NA         |   NA         |  Get size (number of elements) in stack      |

## Artihmetic Instructions
| Instruction     | Arg1      | Arg2          | Arg3         | Description                                  |
|---              |---        |---            |---           |---                                           |
|     add         |        r  |    r , *n     |   r , *n     |  Add Arg2 and Arg3, Store in Arg1            |
|     sub         |        r  |    r , *n     |   r , *n     |  Sub Arg3 from Arg2, Store in Arg1           |
|     mul         |        r  |    r , *n     |   r , *n     |  Mul Arg2 and Arg3, Store in Arg1            |
|     div         |        r  |    r , *n     |   r , *n     |  Div Arg2 by  Arg3, Store in Arg1            |
|     add.d       |        r  |      r        |     r        |  Add (double) Arg2 and Arg3, Store in Arg1   |
|     sub.d       |        r  |      r        |     r        |  Sub (double) Arg3 from Arg3, Store in Arg1  |
|     mul.d       |        r  |      r        |     r        |  Mul (double) Arg2 and Arg3, Store in Arg1   |
|     div.d       |        r  |      r        |     r        |  Div (double) Arg3 by  Arg2, Store    in Arg1   |
   
Arithmetic instructions that specify 'd' assumes that the values being operated on are double-precision floating point
numbers, if the value in a given 'd' register is not a floating point, the behavior is undefined.

## Bitwise Instructions
| Instruction     | Arg1      | Arg2          | Arg3         | Description                                  |
|---              |---        |---            |---           |---                                           |
|     lsh         |        r  |    r , *n     |   r , *n     |  Left shift arg 2 by arg3, store in arg 1    |
|     rsh         |        r  |    r , *n     |   r , *n     |  Right shift arg2 by arg3, store in arg 1    |
|     and         |        r  |    r , *n     |   r , *n     |  And arg 2 with arg 3 store in arg 1         |
|     or          |        r  |    r , *n     |   r , *n     |  Or arg 2 with arg 3, store in arg 1         |
|     xor         |        r  |    r , *n     |   r , *n     |  Xor arg 2 with arg 3, store in arg 1        |
|     not         |        r  |      r        |     NA       |  Flip the bits of arg2 store in arg 1        |


## Branch Instructions

| Instruction   | Arg1           | Arg2             | Arg3               | Description                               |
|---            |---             |---               |---                 |---                                        |
|      bgt      |  r             |   r              |   label            | Branch to Arg3 if Arg1 > Arg2             |
|      bgte     |  r             |   r              |   label            | Branch to Arg3 if Arg1 >= Arg2            |
|      blt      |  r             |   r              |   label            | Branch to Arg3 if Arg1 <= Arg2            |
|      blte     |  r             |   r              |   label            | Branch to Arg3 if Arg1 <= Arg2            |
|      beq      |  r             |   r              |   label            | Branch to Arg3 if Arg1 == Arg2            |
|      bne      |  r             |   r              |   label            | Branch to Arg3 if Arg1 != Arg2            |
|      bgt.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 > Arg2    |
|      bgte.d   |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 >= Arg2   |
|      blt.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 <= Arg2   |
|      blte.d   |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 <= Arg2   |
|      beq.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 == Arg2   |
|      bne.d    |  r             |   r              |   label            | Branch (double) to Arg3 if Arg1 != Arg2   |

Branch instructions assume that the conditional values stored in registers are integer values unless 'd' is specified. 
If 'd' is specified and the value in a given register is not a floating point, the behavior is undefined.

## Loading / Storing Instructions
|  Instruction     |  Arg1     |  Arg2    |  Description                                 |
|---               |---        |---       |---                                           |
|      mov         |    r      |   r, *n  |  Move data from Arg2 into Arg1 (8-bit max *n)|
|      ldw         |    r      |   *sp    |  Load word from Arg2 (address) into Arg1     |
|      stw         |   *sp     |   r      |  Store Arg2 word at Arg1 (arg1=addr)         |
|      ldb         |    r      |   *sp    |  Load data from Arg2 (address) into Arg1     |
|      stb         |   *sp     |   r      |  Store Arg2 data at Arg1 (arg1=addr)         |
|      push        |    sp     |   r      |  Data from Arg2 into Arg1                    |
|      pop         |    r      |   sp     |  Data from Arg2 into Arg1                    |
|      pushw       |    sp     |   r      |  Word (8 bytes) from Arg2 into Arg1          |
|      popw        |    r      |   sp     |  Word (8 bytes) from Arg2 into Arg1          |

## Jump / Call

| Instruction | Arg1     | Description                                       |
|---          |---       |---                                                |
| jmp         | label    | Jump to label                                     |
| call        | function | Call function - return address stored call stack  |
| pcall       | function | Parallel call - Spawns a new execution context    |
| ret         |          | Return to the address stored on top of call stack |
| yield       |          | Yield operation of function to caller             |

## Exit

| Instruction | Description                                   |
|---          |---                                            |
| exit        | Quit execution of program                     |

## Functions / Labels

| Instruction    | Description                          |
|---             |---                                   |
| <funcName:     | Create function 'funcName'           |
| >              | End of function declaration          |
| labelName:     | Create label 'labelName'             |

## Instruction Data

Each full instruction is 8 bytes. 

### Arithmetic operations

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

**Note:** Since drop-ins are not allowed on double-based arithmetic operations, the id bits listed above
and displayed below are not applicable. Instead, double-based arithmetic operations will default to the 
'00' case listed below.

 **Note** : Numerical constants are limited to the range of a signed 16-bit integer

Here is an example of the bit layout given an arithmetic operation. Note: All but ID here are filled
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

    Case 11:
    INS    ID   REGISTER    [ ---- INTEGER ---- ]   [ ---- INTEGER ---- ]   [ ---- UNUSED ---- ]
    111111 11 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111
    
## Bitwise Operations

[Byte 1]
The first 6 bits represent the specific instruction (add / mul/ etc)
The remaining 2 bits of the first byte indicate what the remaining bytes represent.

The indication bits are as follows:
00 - Byte 3 will be a register, Byte 4 will be a register
01 - Byte 3 will be a register, Byte 4/5 will be a 2-byte integer
10 - Byte 3/4 will be a 2-byte integer, Byte 5 will be a register
11 - Byte 3/4 will be a 2-byte integer, Byte 5/6 will be a 2-byte integer

 **Note** : Numerical constants are limited to the range of a signed 16-bit integer

[Byte 2]
The second byte is the destination register

Unaccounted bytes will be unused, and marked as '1'

Here is an example of the bit layout given a bitwise operation. Note: All but ID here are filled
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

    Case 11:
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

**mov** - Move

Indication Bits:
00 - Register, Register
01 - Register, Numerical Constant - **Note** : This numerical constant is limited to the range of a signed 32-bit integer.

    INS    ID   REGISTER    REGISTER    [ ----------------------- UNUSED ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    INS    ID   REGISTER    [ ---------------  INTEGER ------------------ ] [ ---- UNUSED ----- ]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**stb** - Store byte

Indication Bits:
00 - Stack with numerical offset, Source Register
01 - Stack with register-base offset, Source Register

    INS    ID    STACK       [ ---------------   ADDRESS  ---------------]   REGISTER    UNUSED
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    INS    ID    STACK      REGISTER     REGISTER   [ ------------------ UNUSED ----------------]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**ldb** - Load byte

Indication Bits:
00 - Destination Register, Stack with numerical offset
01 - Destination Register, Stack with register-base offset

    INS    ID   REGISTER      STACK     [ ---------------   ADDRESS  ---------------]    UNUSED
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    INS    ID    REGISTER      STACK     REGISTER   [ ------------------ UNUSED ----------------]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**stw** - Store word (8 bytes)

Indication Bits:
00 - Stack with numerical offset, Source Register
01 - Stack with register-base offset, Source Register

    INS    ID    STACK       [ ---------------   ADDRESS  ---------------]   REGISTER    UNUSED
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    INS    ID    STACK      REGISTER     REGISTER   [ ------------------ UNUSED ----------------]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**ldw** - Load word (8 bytes)

Indication Bits:
00 - Destination Register, Stack with numerical offset
01 - Destination Register, Stack with register-base offset

    INS    ID   REGISTER      STACK     [ ---------------   ADDRESS  ---------------]    UNUSED
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

    INS    ID    REGISTER      STACK     REGISTER   [ ------------------ UNUSED ----------------]
    111111 01 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**push** - Push

    INS    ID      STACK     REGISTER   [ ------------------- UNUSED -------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**pop** - Pop

    INS    ID   REGISTER      STACK     [ --------------------   UNUSED  ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**pushw** - Pushw (8 bytes)

    INS    ID      STACK     REGISTER   [ ------------------- UNUSED -------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**popw** - Popw (8 bytes)

    INS    ID   REGISTER      STACK     [ --------------------   UNUSED  ---------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

### Jump/ call/ pcall/ ret/ yield operations

**jmp** - Jump

The jump operation is straight forward. The only data in the jump is the address to jump to.

    INS    ID   [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**call** - Call

Upon executing the call instruction, the address immediately after call's address will be stored in
the system stack. 

    INS    ID   [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111
    
**pcall** - pcall

Upon executing the pcall instruction, the destination address will be sent to the virtual machine to
have a new context spawned at that address location. The VM will stay alive until all contexts are completed. 
The context will be completed once the new context's call stack is empty and instructions from the destination
address are completed.

    INS    ID   [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**ret** - Return

The return address will be on the top of the system stack. Executing return will return to whatever
address is on the top of the system stack, and then pop it. 

    INS    ID  [ ----------------------------------- UNUSED ----------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

Note: Implicit returns happen at the bottom of a function. If the bottom of the function is reached, 
a return will occur.

**yield** - Yield

Yields execution back to caller. Upon detecting yeild, the function's instruction pointer and local
stack is preserved and the caller continues execution as if a return occured. Upon the next call into
the function that has yielded, execution will continue from the instruction immediately following the
yeild. Multiple yeilds can me made, and if the bottom of the function occurs, or if a ret is executed
the function will be reset. 

    INS    ID  [ ----------------------------------- UNUSED ----------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111


## Misc Instructions

Nop

    INS    ID   [ --------------------------------------   UNUSED  ---------------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

Size

    INS    ID   REGISTER    STACK      [ ---------------------------- UNUSED -------------------------------]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

## Constants

Constant creation instructions don't follow standard instruction layout which is why they were saved for last.

**.int**

id bits: 00 - 8-bit  int [1 byte  following ins byte]
id bits: 01 - 16-bit int [2 bytes following ins byte]
id bits: 10 - 32-bit int [4 bytes following ins byte]
id bits: 11 - 64-bit int [8 bytes following ins byte]

    INS    ID   [ --- INTEGER BYTES ---- ]
    111111 00 | 1111 1111 .... | 1111 1111

**.string**

id bits ignored

The current maximum is 255 bytes for a string

    INS    ID    SIZE      [ ------- STR BYTES ------ ]
    111111 00 | 1111 1111 | 1111 1111 | .... | 1111 1111

**.double**

id bits ignored, size is 9 bytes fixed

    INS    ID   [ -------------------------------------- DOUBLE DATA -------------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

## The stack

In this system, there are multiple stacks, but only 2 that can be accessed at any given time programatically. The local stack, and the global stack.

### Local stack

A stack accessed by 'ls' for 'local stack' that accesses the stack preserved for the current function. Once the system leaves the function, the local stack is blown away. 

### Global stack

A stack accessed by 'gs' for 'global stack' that accesses the stack used across the entire program. Great care should be taken when using this stack, as there is no garbage collection in the scope of what I care to do here.

### Call stack

Not able to be accessed by software directly. The system stack is pushed and popped by calls and returns.

## Forbidden Instructions 

There are instructions that exist that are required by the system that the user does not need to worry about, but they need to be
documented for the sake of the implementation.

**function start** - Currently allows 2^48 instructions per function


Function start / end are special instructions that are scanned for upon initial program load
to tell the VM how to structure its self. 

    INS    ID   [ --------------------- NUM INSTRUCTIONS IN FUNC ------------------------------ ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**function end**

    INS    ID   [ ---------------------------------- UNUSED ----------------------------------- ]
    111111 00 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**callstack_store_function**

Tell the system to put a function address in the call stack

      INS      [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    11111111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**callstack_store_roi**

Tell the system to put a function location in the call stack

      INS      [ ---------------   ADDRESS  --------------- ]  [ ---------- UNUSED ----------- ]
    11111111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

These instructions are used in the back-end to make calls and returns work. When a call is written by a user, these are generated
by the byte gen to tell the system to put the function and region the call came from such-that returns can identify where to go to

**begin_constant_segment**

Tell the system that the following area is the binary is constants

      INS      [ ---------------------------------- Number of constants ---------------------------------- ]
    11111111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**begin_function_segment**

Tell the system that the following are is functions.

      INS      [ ---------------------------------- Entry Function Address ------------------------------- ]
    11111111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111

**binary_end**

Right now the data is unused, but eventually the unused section will contain a binary checksum

      INS      [ ----------------------------------- Unused ---------------------------------- ]
    11111111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111