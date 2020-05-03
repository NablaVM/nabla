.file "net_tcp"

.init main

<main:

    call create_tcp_in
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

    ; This needs to be done later - have to stop working rn
    ; lsh r1 

    ; Move command into r10
    mov r10 r0
>