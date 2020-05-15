# Nabla Devices

## Register Key

Devices that are used by the Nabla VM will key off of register 10. At the end of an instruction cycle, if a device sees information regarding its operation in r10 it will act accordingly. 

### What does this mean ? 

Don't use registers 10 - 15 without a clear goal. They are reserved for device operations

## Devices


| Device     | ID      | Description                 |  
|---         |---      |---                          |  
|    IO      | 0x0A    | User and disk io operations |
| Network    | 0x0B    | TCP/UDP Network operations  |
| Host       | 0x0C    | Host operations             |


# Register 10 Decodes

Which device to be triggered is the most significant byte of the register. The remainder of the register
is different depending on the ID placed in the register. 

## IO Device

IO Device trigger register and state structure.

           ID         STATE    [ ----------------------- Varies by target  ----------------------- ]
        0000 1010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

**ID**     - The ID of the IO device (0x0A) 

**STATE** - The targeted state instruction for the device

States: 

| State       | ID    | Description                  |  
|---          |---    |---                           |  
|    stdin    |  0    | Invoke standard in           |
|    stdout   |  1    | Invoke standard out          |
|    stderr   |  2    | Invoke standard error        |
|    diskin   |  100  | Invoke disk in (file in)     |
|    diskout  |  101  | Invoke disk out (file out)   |
|    close    |  200  | Close the current in/output  |
|    report   |  255  | Report what state is set     |

### IO - User IO

If the IO device state is not in a 'close' state, that means that DISK IO is occuring, and user IO will be rejected. This is a safety mechanism that must be kept in mind while operating with the IO device.

The use of the word STATE can get confusing here. The second byte is the STATE byte, and directs input. For commands that have instructions, the STATE acts like a router. The instruction goes to that section of device IO if STATE aligns. If the IODevice is in a close STATE and a a new command comes in, the IODevice will switch to the given STATE.

**report**

This command will report to r11 what state is set (diskin, diskout, etc) without changing the state of the IODevice. Provided that the ID and STATE bytes are set correctly, this shall always return the ID value of the current state to r11.

           ID         STATE     [ --------------------------- UNUSED -------------------------------]
        0000 1010 | 1111 1111 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

**stdin** 

Reads from standard input a up-to a specified number of bytes and won't return until TERMINATION byte is read in. Bytes read in will be pushed onto the global stack,
with the number of bytes read in stored in r11. Once bytes are read in, r10 will be cleared and the IODevice will be in a 'closed' state.

           ID         STATE     [ ---------- NUM BYTES TO READ ------------ ]   [ TERM  ]   [ UNUSED ]
        0000 1010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

**stdout** 

           ID         STATE     [ --------------------------- UNUSED -------------------------------]
        0000 1010 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

**stderr**

           ID         STATE     [ --------------------------- UNUSED -------------------------------]
        0000 1010 | 0000 0010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

The output states (stdout & stderr) operate the same, while directing to different output streams. They will write the contents of 
r11 to the specified stream, and then put the IO Device into a 'closed' state. This that outputting lengths of data must happen in
segments of 8 bytes.

### IO - Disk IO

The disk IO functionality operates differently than user io in-that it must be opened and closed explicitly. In addition to that difference, the disk IO has a small set of commands to perform different disk operations that must be loaded into vm registers to operate correctly.

As an example, if the disk is asked to be opened the operation will occur, then the trigger register will be set to 0, and a new command to read will need to occur. Once that read occurs the trigger register will be set to 0 and so-on. Once the read is determined complete by the application the close command must be sent. If a differnet IO command is sent to the IO device while the IO is set to read from disk, the command will be rejected.

To review : 
    
    If DiskIn is set, all other IO operations will be blocked until a close instruction is given to the IO Device. 

    If DiskOut is set, all other IO operations will be blocked until a close instruction is given to the IO Device.

#### Diskin

* Register 10

           ID         STATE      INSTRUCT   [ ---------------- INSTRUCTION PARAMS ----------------- ]
        0000 1010 | 0110 0100 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000


Disk In Commands

| Instruction | Value | Description                               |  
|---          |---    |---                                        |  
|    open     |  1    | Open disk location                        |
|    read     |  10   | Read specified number of bytes            |
|    seek     |  20   | Seek to a location in the file            |
|    rewind   |  30   | Set file pointer to start of file         |
|    tell     |  40   | Request the current file pointer location |

Closing the disk is done via the IO Device, via the 'close' target.

**diskin - open**


* Register 10

           ID         STATE      INSTRUCT   [ -------------------------- UNUSED --------------------]
        0000 1010 | 0110 0100 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000

* Register 11

        [ --------- GLOBAL STACK START ADDRESS ----- ]  [ --------- GLOBAL STACK END ADDRESS ----- ]
        0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000

Open will read a file name from the address range specified in register 11. If the file is not found and 
can not be opened, register 11 will contain '0' after execution, otherwise it will contain '1'
If failure to open occurs, the IO Device will be placed in a 'close' state.

**diskin - read**


* Register 10

           ID         STATE      INSTRUCT   [ ------------- NUM BYTES ------------------]  [ UNUSED ]
        0000 1010 | 0110 0100 | 0000 1010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000


The number of bytes read in will be reported into r11.
If a fail to read happens r11 and r12 will still be reported indicating how much data they received before
the fail to read occurs. The device will stay open.

**diskin - seek**

* Register 10

           ID         STATE      INSTRUCT   [ ------------- LOCATION -------------------]  [ UNUSED ]
        0000 1010 | 0110 0100 | 0001 0100 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000


If operation is okay, r11 will have a value of '1', otherwise it will have a value of '0'


**diskin - rewind**

* Register 10

           ID         STATE      INSTRUCT   [ ------------- LOCATION -------------------]  [ UNUSED ]
        0000 1010 | 0110 0100 | 0001 1110 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000


If operation is okay, r11 will have a value of '1', otherwise it will have a value of '0'


**diskin - tell**

* Register 10

           ID         STATE      INSTRUCT   [ ------------------------ UNUSED --------------------- ]
        0000 1010 | 0110 0100 | 0010 1000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000


If operation is okay, r11 will have the value of the file pointer location


#### Diskout


* Register 10

           ID         STATE      INSTRUCT   [ ---------------- INSTRUCTION PARAMS ----------------- ]
        0000 1010 | 0110 0100 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000


Disk In Commands

| Instruction | Value | Description                           |  
|---          |---    |---                                    |  
|    open     |  1    | Open disk location                    |
|    write    |  10   | Read specified number of bytes        |

Closing the disk is done via the IO Device, via the 'close' state command.


**diskout - open**


* Register 10

           ID         STATE      INSTRUCT      MODE     [ ------------ UNUSED ----------------------] 
        0000 1010 | 0110 0101 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000

* Register 11

        [ --------- GLOBAL STACK START ADDRESS ----- ]  [ --------- GLOBAL STACK END ADDRESS ----- ]
        0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000

Open will read a file name from the address range specified in register 11. If the file is not found and 
can not be opened, register 11 will contain '0' after execution, otherwise it will contain '1'
If failure to open occurs, the IO Device will be placed in a 'close' state.

Open modes can be as follows : 

| Mode                | Value | Description                               |  
|---                  |---    |---                                        |  
|    write            |  1    | Open / create. Overwrite exisitng         |
|    append           |  2    | Open for append                           |
|    append/create    |  3    | Open for append / create if doesn't exist |


**diskout - write**

* Register 10

           ID         STATE      INSTRUCT   [ ------------------------ UNUSED ----------------------] 
        0000 1010 | 0110 0101 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000


This will write whatever exists in r11 to file and unset register 10. Writing won't continue until register
10 is given another disckout-write command. 

Register 11 will be written to '1' upon completion

