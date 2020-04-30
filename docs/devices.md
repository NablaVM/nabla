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


## Register 10 Decodes

Which device to be triggered is the most significant byte of the register. The remainder of the register
is different depending on the ID placed in the register. 

**IO Device** - Input and output

The IO device will switch to the target given if it differs from its current state. Care should be taken here to ensure nothing silly happend. Every open should have a matching close. Using this can be like playing with fire. 

           ID         TARGET    [ ----------------------- Varies by target  ----------------------- ]
        0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Targets: 

| Target      | ID    | Description                  |  
|---          |---    |---                           |  
|    stdin    |  0    | Invoke standard in           |
|    stdout   |  1    | Invoke standard out          |
|    stderr   |  2    | Invoke standard error        |
|    diskin   |  3    | Invoke disk in (file in)     |
|    disckout |  4    | Invoke disk out (file out)   |
|    close    |  5    | Close the current in/output  |
|    none     |  6    | Not something you should set |


stdin 

Reads from standard input a up-to a specified number of bytes and won't return until TERMINATION byte is read in. Bytes read in will be pushed onto the global stack,
with the number of bytes read in stored in r11, and number of stack frames generated in r12 Once bytes are read in, 
r10 will be cleared and the IODevice will target nothing.

           ID         TARGET    [ ---------- NUM BYTES TO READ ------------ ]   [ TERM  ]   [ UNUSED ]
        0000 1010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

stdout 

           ID         TARGET    [ --------------------------- UNUSED -------------------------------]
        0000 1010 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

stderr

           ID         TARGET    [ --------------------------- UNUSED -------------------------------]
        0000 1010 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 


IO Device will take the bytes in register r11 and output to whatever target is given for the TARGET byte until the target byte indicates a 'close.' Ensure that all data is passed before close, as once close is found, the device will be closed and the data currently in r11 will be ignored. 

In the future, for file operations we might update the unused section to drive seek operations, but for now that functionality is being put off until a solid POC is implemented. 

**Network Device** - Network device

Networking hasn't been started let alone figured out. The plan is to have it be setup similar to IO with some socket action. This will come later. 

           ID         TARGET    [ ----------------------- UNUSED ---------------------------------- ]
        0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 