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

## Network Device

The network device will support TCP and UDP over IPv4. IPv6 is being put off for the forseeable future. 

NET Device trigger register and state structure.

           ID         SUB-ID    [ ----------------------- Varies by SUB-ID  ----------------------- ]
        0000 1011 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 
        
**ID**     - The ID of the Network Device (0x0B)

**SUB-ID** - The an ID that directs the rest of the command specific to a particular functionality

SUB-IDs: 

| Sub Id          | VALUE | Description                           |  
|---              |---    |---                                    |  
|    netouttcp    |  0    | Network outbound using tcp            |
|    netintcp     |  1    | Network inbound  using tcp            |
|    netudp       |  10   | Network outbound using udp            |
|    shutdown     |  50   | Close and release all network objects |
|    restart      |  55   | Shutdown, and reset network device    |

**Overview**

The network device allows the creaton of 'network objects' that are of a SUB-ID type and have unique IDS. Each network object facilitates an inbound or outbound action over tcp or udp. Creation of these objects are specified below. Objects are either created by the user, or they are created in response to an 'accept' from a netintcp.

Unlike the IO Device, the NET Device can operate multiple objects at once and this creates a bit more complexity. 

## **Shared Commands**

| Command         | VALUE | Description                             |  
|---              |---    |---                                      |  
|    create       |  0    | Create an object                        |
|    delete       |  1    | Delete an object                        |
|    close        |  2    | Close the object                        |

After the execution of each shard command, register 10 will be wiped. 

**Shared command - create**

* Register 10

           ID         SUB-ID     COMMAND      DOMAIN      TYPE       PROTOCOL   [ ----- PORT ------ ]  
        0000 1011 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

* Register 11

         [ ---------------IP ADDRESS ----------------- ]   BLOCK     [ ----------- UNUSED ---------- ]
         0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

DOMAIN, TYPE, and PROTOCOL are the standard socket setup (AF_INET, SOCK_STREAM | DGRAM, etc)
PORT and ADDRESS are the port and address assigned to the object. What the port and address mean depend on the
SUB-ID. For instance, netouttcp will use this to reach out, while netintcp will use it to bind locally for 
connection listening. BLOCK indicates if the underlying socket should be set to non-blocking or not (1 for blocking, 0 otherwise)

Upon processing the create command, register 10 and 11 will be cleared. The result of the create command
will be stored in r11 as follows :

* Result - Register 11

           RESULT    [ ---- Object ID ---- ] [ --------------------- UNUSED ------------------------ ]
         0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

If the create command was a success, the RESULT byte will be '1', otherwise, it will be '0'. If the result
command was a success, then the new network object's id will be stored in the follwing 2 bytes. 

This object id is how all commands will be directed to a particular object.

**Shared command - delete**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ --------- UNUSED ------------ ]
        0000 1011 | 0000 0000 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

The delete command will close out the network object, and free it from memory. If the given object does
not exist, nothing will happen. 

**Shared command - close**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ --------- UNUSED ------------ ]
        0000 1011 | 0000 0000 | 0000 0010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

The close command will close-out whatever connection that the object is operating on and ready it to be
re-bound, re-connected, or whatever makes sense for that SUB-ID.


## **netintcp commands**

| Command         | VALUE | Description                             |  
|---              |---    |---                                      |  
|    bind         |  10   | Bind object to socket                   |
|    listen       |  11   | Set object to listen for connections    |
|    accept       |  12   | Accept a connection from remote         |

**bind** 

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ --------- UNUSED ------------ ]
        0000 1011 | 0000 0001 | 0000 1010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Binds the given network object to a socket as-per the parameters of its initial creation. 

If the bind command was a success, register 11 will read '1', otherwise '0'.

**listen**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ ---- BACK LOG --- ]     UNUSED
        0000 1011 | 0000 0001 | 0000 1011 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

The BACK LOG is the number of back-logged requests is permitted on listen. If the command is a success, 
register 11 will read '1', otherwise '0'

**accept**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ --------- UNUSED ------------ ]
        0000 1011 | 0000 0001 | 0000 1100 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

If a connection is accepted, a new netouttcp object will be spawned so we can communicate with the remote
destination.

* Result - Register 11

           RESULT    [ ---- Object ID ---- ] [ ----------------------- UNUSED ---------------------- ]
         0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

If the result is a success the RESULT byte will be '1', otherwise it will be '0'. If the RESULT byte is '1'
then the new nettcpout object's ID will be in the following 2 bytes.


## **netouttcp commands**

| Command         | VALUE | Description             |  
|---              |---    |---                      |  
|    connect      |  20   | Connect to a remote     |
|    send         |  21   | Send data to a remote   |
|    receive      |  22   | Receive from a remote   |

**connect**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ --------- UNUSED ------------ ]
        0000 1011 | 0000 0000 | ‭0001 0100‬ | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Requests a connection to the remote as-per the information given to the object upon its creation.
If it is a success, r11 will contain a '0' if it fails, it will be '1'

**send**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ ---- NUM BYTES ---- ]   UNUSED
        0000 1011 | 0000 0000 | ‭0001 0101 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

* Register 11

        [ --------- GLOBAL STACK START ADDRESS ----- ]  [ --------- GLOBAL STACK END ADDRESS ----- ]
        0000 1011 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

In register 10 NUM BYTES is the number of bytes that are to be sent out. Register 11 contains the start
and end address of the global stack where the bytes are stored. If NUM BYTES is larger than what could
be contained by the start and end address given, the send will be cancelled and an error will be reported
in r11.

The network object will attempt to send the information given the settings. If it is a success, r11 will
contain a '0' if it fails, it will be '1'

**receive**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ ---- NUM BYTES ---- ]   UNUSED
        0000 1011 | 0000 0000 | ‭0001 0110 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

* Register 11

        [ --------- GLOBAL STACK START ADDRESS ----- ]  [ --------- GLOBAL STACK END ADDRESS ----- ]
        0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

In register 10 NUM BYTES is the number of bytes allocated to be received. Register 11 contains the start
and end address of the global stack where the bytes will be stored. If NUM BYTES is larger than what 
could be contained by the global stack given the addresses, the receive will be cancelled and an error
will be reported in r11.

The network object will attempt to receive information up-to the size given in NUM BYTES. If it is a success
r11 will contain the number of bytes produced from recv, otherwise it will be '0'

## **netudp commands**

| Command         | VALUE | Description             |  
|---              |---    |---                      |  
|    bind         |  70   | Bind object to socket   |
|    send         |  71   | Send data to a remote   |
|    receive      |  72   | Receive from a remote   |

**bind**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ --------- UNUSED ------------ ]
        0000 1011 | 0000 1011 | 0100 0110 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Binds the given network object to a socket as-per the parameters of its initial creation. 

If the bind command was a success, register 11 will read '1', otherwise '0'.

**send**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ ---- NUM BYTES ---- ]   UNUSED
        0000 1011 | 0000 1011 | 0100 0111 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

* Register 11

        [ --------- GLOBAL STACK START ADDRESS ----- ]  [ --------- GLOBAL STACK END ADDRESS ----- ]
        0000 1011 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

* Register 12 

        [ -------------------------------- UNUSED -------------------------- ] [  Remote Object ID  ]
        0000 1011 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

In register 10 NUM BYTES is the number of bytes that are to be sent out. Register 11 contains the start
and end address of the global stack where the bytes are stored. If NUM BYTES is larger than what could
be contained by the start and end address given, the send will be cancelled and an error will be reported
in r11.

r12 is the socket used to identify the remote object

The network object will attempt to send the information given the settings. If it is a success, r11 will
contain a '1' if it fails, it will be '0'

**receive**

* Register 10

           ID         SUB-ID     COMMAND    [ ---- Object ID ---- ] [ ---- NUM BYTES ---- ]   UNUSED
        0000 1011 | 0000 1011 | 0100 1000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

* Register 11

        [ --------- GLOBAL STACK START ADDRESS ----- ]  [ --------- GLOBAL STACK END ADDRESS ----- ]
        0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

* Register 12 

        [ -------------------------------- UNUSED -------------------------- ] [  Remote Object ID  ]
        0000 1011 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

In register 10 NUM BYTES is the number of bytes allocated to be received. Register 11 contains the start
and end address of the global stack where the bytes will be stored. If NUM BYTES is larger than what 
could be contained by the global stack given the addresses, the receive will be cancelled and an error
will be reported in r11.

r12 is the socket used to identify the remote object

The network object will attempt to receive information up-to the size given in NUM BYTES. If it is a success
r11 will contain number of bytes produced by recv, otherwise it will be '0'


## **shutdown**

**shutdown**

* Register 10

           ID         SUB-ID    [ ----------------------------- UNUSED ---------------------------- ]
        0000 1011 | ‭0011 0010‬ | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Closes and clears all network objects. Places the network device as 'inactive' and will not process any further 
commands (except restart)

## **restart**

**restart**

* Register 10

           ID         SUB-ID    [ ----------------------------- UNUSED ---------------------------- ]
        0000 1011 | ‭0011 0111‬ | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

Closes and clears all network connections. Restarts the device such-that the device can begin operating again. 
All connections and connection info will be lost. 