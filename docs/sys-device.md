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

## Host Device

NET Host trigger register and state structure.

           ID         SUB-ID    [ ----------------------- Varies by SUB-ID  ----------------------- ]
        0000 1100 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 
        
**ID**     - The ID of the Host Device (0x0C)

**SUB-ID** - The an ID that directs the rest of the command specific to a particular functionality

SUB-IDs: 

| Sub Id          | VALUE | Description                                         |  
|---              |---    |---                                                  |  
|    clock        |  0    | Clock ticks elapsed since the start of the program  |
|    clocks ps    |  1    | Clock ticks per second                              |
|    epoch time   |  10   | Seconds since 00:00:00 UTC January 1, 1970          |
|    random       |  20   | Get a pseudo random number                          |

**Overview**

The system command is a set of functionality that allows the vm to interact with a small set of the host system. 


**clock**

* Register 10

           ID         SUB-ID    [------------------------------ UNUSED -----------------------------]
        0000 1100 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Get the elapsed time since the start of the program. Result will be in r11.


**clock ps**

* Register 10

           ID         SUB-ID    [------------------------------ UNUSED -----------------------------]
        0000 1100 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Clockes per second. This is host defined, result will be in r11.


**epoch time**

* Register 10

           ID         SUB-ID    [------------------------------ UNUSED -----------------------------]
        0000 1100 | 0000 1010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Get the time since 00:00:00 UTC January 1, 1970 in seconds. Result will be in r11.

**random**

* Register 10

           ID         SUB-ID    [------------------------------ UNUSED -----------------------------]
        0000 1100 | 0001 0100 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Generate a pseudo random number and store in r11.