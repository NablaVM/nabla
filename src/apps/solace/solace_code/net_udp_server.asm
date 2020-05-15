;
;   This UDP server can receive a single connection. It receives upto 2000 bytes, 
;   prints the data on the screen, and responds with "Hello client, I am server"
;   This can be used with the libc/sockets/example/udpclient.c
;

.file "net_udp"

.init main

.string setupError "Failed to create the socket" ; 0 - 27
.string exiting    "Exiting"                     ; 27 - 34
.string newConn    "New connection!"             ; 34 - 49
.string createdSoc "Socket created!"             ; 49 - 64
.string bindErr    "Error binding socket"        ; 64 - 84
.string listenErr  "Error setting listen"        ; 84 - 104
.string starting   "Starting server"             ; 104 - 119
.string serverStr  "Hello client, I am server"   ; 119 - 144
.string recvErr    "Error receiving data"        ; 144 - 164
.string recvGood   "Client sent us some data!"   ; 166 - 193

<main:

    call create_udp_socket

    ; Put new id in r0 and store again
    popw r0 gs 
    pushw gs r0

    ; Bind socket in r0 so it can listsn 
    call bind_udp_socket

    ; Create a udp socket for sending data
    call create_udp_socket

    ; Top 2 items in stack are sockets. 

    call listen
>

;
;   Display message to user. expects gs start in r5, and end in r6
;
<println:

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r0 r0 r1 

    mov r1 r6
    mov r2 r5

loop_top:

    ldb r11 r2(gs)  ; Load from gs index

    mov r10 r0      ; Trigger stdout

    add r2 r2 $1    ; add one to conuter 

    blt r2 r1 loop_top

    ; Print a new line 

    mov r11 $10       ; 10 Happens to be the value of the NL character
    mov r10 r0        ; Trigger stdout to display new line
>

;
;   Shutdown the network device 
;      - Only restart will be permitted once device is in shutdown state
;
<shutdown_network_device:

    lsh r0 $11 $56  ; Network device
    lsh r1 $50 $48  ; Shutdown

    or r10 r0 r1    ; Combine the registers into exec reg
>

;
;   Restart the network device (from active, or shutdown state)
;      - Only if restart fails (sys out of memory) will device be inoperable after restart
;
<restart_network_device:
    
    lsh r0 $11 $56  ; Network device
    lsh r1 $55 $48  ; Restart

    or r10 r0 r1    ; Combine the registers into exec reg
>

;
;   Create the server socket
;
<create_udp_socket:

    ; Create first half of command

    lsh r0 $11 $56  ; Network device
    lsh r1 $10 $48  ; netudp
    lsh r2 $0  $40  ; create 
    lsh r3 $2  $32  ; AF_INET
    lsh r4 $2  $24  ; SOCK_DGRAM
    lsh r5 $0  $16  ; protocol
    lsh r6 $4098 $0 ; port

    or r0 r0 r1     ; Assemble into r0
    or r0 r0 r2 
    or r0 r0 r3 
    or r0 r0 r4 
    or r0 r0 r5 
    or r0 r0 r6 

    ; Set IP address - INADDR_ANY

    lsh r1 $0 $56 
    lsh r2 $0 $48
    lsh r3 $0 $40
    lsh r4 $0 $32

    or r1 r1 r2     ; Assebme into r1
    or r1 r1 r3
    or r1 r1 r4

    lsh r2 $1 $24   ; Set blocking

    or r1 r1 r2     ; Put blocking info in

    ; Move address and block flag to r11 for processing
    mov r11 r1 

    ; Move command into r10 to process command
    mov r10 r0

    ; -- Check result --

    mov r0 $0

    bne r0 r11 success

    ; If it was not a success, we fall into exit :(

    ; Tell the user we failed
    mov r5 $0 
    mov r6 $4
    call println

    ; Tell the user we are exiting
    mov r5 $27
    mov r6 $34
    call println

    exit 

success:

    lsh r0 r11  $8      ; Get rid of result byte
    rsh r0 r0  $48      ; Move id to LSB
    
    pushw gs r0          ; Store the object id on the global stack

    ; Tell the user we created the socket

    mov r5 $49 
    mov r6 $64
    call println

    ret
>

;
;   Bind socket id in r0
;
<bind_udp_socket:

    mov r9 r0

    lsh r0 $11 $56  ; Network device
    lsh r1 $10 $48  ; netudp
    lsh r2 $70 $40  ; bind 
    lsh r3 r9  $24  ; socket id

    pushw gs r9 ; Store the object again

    or r0 r0 r1 
    or r0 r0 r2
    or r0 r0 r3

    mov r10 r0  ; Execute the bind command

    ; Check result
    mov r0 $0
    bne r0 r11 success 

    ; Error binding socket - tell user
    mov r5 $64
    mov r6 $84
    call println

    ; Tell the user we are exiting
    mov r5 $27
    mov r6 $34
    call println

    exit

success:
    ret
>

;
;   Listen for data in
;
<listen:

    popw r0 gs ; Should be sending socket 
    popw r1 gs ; Should be recv socket 

    ; Save in local stack
    pushw ls r1 
    pushw ls r0

    ; Get current size of gs
    size r1 gs 
    pushw ls r1 

    ; Expand global stack to store data 
    mov r1 $0
    mov r2 $0
    add r3 $200 $50

expand_stack:
    pushw gs r1
    add r2 r2 $1
    blt r2 r3 expand_stack

    ; Get new size of gs
    size r6 gs 
    
    popw r7 ls   ; old gs size 
    popw r8 ls   ; Recv socket 
    popw r9 ls   ; Send socket

    ; Make the r11 global stack indexing
    lsh r7 r7 $32
    or r11 r7 r6

    ; Create recv command

    lsh r0 $11 $56  ; Network device
    lsh r1 $10 $48  ; netudp
    lsh r2 $72 $40  ; recv 
    lsh r3 r9  $24  ; socket id
    lsh r4 $2000 $8 ; bytes to recv

    pushw gs r9 ; Store the object again

    or r0 r0 r1 
    or r0 r0 r2
    or r0 r0 r3
    or r0 r0 r4

    mov r12 r8  ; Load send socket

    pushw ls r0  ; recv command 
    pushw ls r12 ; send socket
    pushw ls r8  ; recv socket 

    mov r7 r12 
    mov r8 $0
    mov r9 r11 
loop:

    mov r12 r7  ; Load recv socket 
    mov r11 r9  ; Load gs info
    mov r10 r0  ; Execute the recv command

    ; If r11 isn't 0, a connection is had, add it to local stack
    bne r11 r8 new_connection

    jmp loop

new_connection:

    ; Tell the user we got data
    add r5 $160 $4
    add r6 $190 $3
    call println

    ; Start of the gs data-in

    add r5 r6 $16

    add r6 r6 r11 ; Add number of stack frames produced to the start index


    ; Print the data received to screen 
    call println

    ; Send data back

    popw r8 ls ; recv socket 
    popw r7 ls ; send socket 

    lsh r0 $11 $56  ; Network device
    lsh r1 $10 $48  ; netudp
    lsh r2 $71 $40  ; send 
    lsh r3 r8  $24  ; socket id
    lsh r4 $32 $8   ; bytes to recv

    or r0 r0 r1 
    or r0 r0 r2
    or r0 r0 r3
    or r0 r0 r4

    ; Build stack source info (constant str)
    lsh r3 $119 $32
    lsh r4 $144 $0
    or r11 r3 r4

    mov r12 r7

    mov r10 r0
    ret
>
