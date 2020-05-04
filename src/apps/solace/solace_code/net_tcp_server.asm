.file "net_tcp"

.init main

.string setupError "Failed to create the socket" ; 0 - 4
.string exiting    "Exiting"                     ; 4 - 4
.string newConn    "New connection!"             ; 5 - 7
.string createdSoc "Socket created!"             ; 7 - 9
.string bindErr    "Error binding socket"        ; 9 - 12

<main:
    ; Create the TCP socket - 127.0.0.1 , port 4096
    ; If success, the socket id will be added to gs
    call create_tcp_in

    ; Setup socket 
    call setup_tcp_in




    ; Shutdown
    call shutdown_network_device

    ; Tell the user we are exiting
    mov r5 $4
    mov r6 $4
    call prompt_user
>

;
;   Display message to user. expects gs start in r5, and end in r6
;
<prompt_user:

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
;   Create a TCP inbound network object
;
<create_tcp_in:

    ; Create first half of command

    lsh r0 $11 $56  ; Network device
    lsh r1 $1  $48  ; net in tcp
    lsh r2 $0  $40  ; create 
    lsh r3 $2  $32  ; AF_INET
    lsh r4 $1  $24  ; SOCK_STREAM
    lsh r5 $0  $16  ; protocol
    lsh r6 $4096 $0 ; port

    or r0 r0 r1     ; Assemble into r0
    or r0 r0 r2 
    or r0 r0 r3 
    or r0 r0 r4 
    or r0 r0 r5 
    or r0 r0 r6 

    ; Set IP address

    lsh r1 $127 $56 
    lsh r2 $0   $48
    lsh r3 $0   $40
    lsh r4 $1   $32

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
    call prompt_user

    ; Tell the user we are exiting
    mov r5 $4
    mov r6 $4
    call prompt_user

    exit 

success:

    lsh r0 r11  $8       ; Get rid of result byte
    rsh r0 r0  $48      ; Move id to LSB
    

    push gs r0          ; Store the object id on the global stack

    ; Tell the user we created the socket

    mov r5 $7 
    mov r6 $9
    call prompt_user

    ret
>

;
;   Setup the socket (bind and listen)
;
<setup_tcp_in:

    pop r9 gs ; Get the socket object

    lsh r0 $11 $56  ; Network device
    lsh r1 $1  $48  ; net in tcp
    lsh r2 $10 $40  ; bind 
    lsh r3 r9  $24  ; socket id

    push gs r9 ; Store the object again

    or r0 r0 r1 
    or r0 r0 r2
    or r0 r0 r3

    mov r10 r0  ; Execute the bind command

    ; Check result
    mov r0 $0
    bne r0 r11 success 

    ; Error binding socket - tell user
    mov r5 $9
    mov r6 $12
    call prompt_user

    ; Tell the user we are exiting
    mov r5 $4
    mov r6 $4
    call prompt_user

    exit

success:
    ret
>