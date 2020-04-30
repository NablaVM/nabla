.file "io"
.init main

<main:

    call invoke_stdin

>

<invoke_stdin:

    ;   We have to build a 'close' instruction so we can close the stdin
    ;   with one instruction as stdin is invoked at the end of each cycle!
    ;   
    lsh r14 $10 $56    ; Load the STDIN ID into the MSB of some register
    lsh r15 $5  $48    ; Load the TARGET 'close' into another register
    or r14 r14 r15     ; OR them together so we have a close instruction targeted at stdin

    push ls r14        ; Push the close instruction to the local stack for later use

    mov r0 $1
    mov r1 $1
    
    lsh r10 $10 $56    ; Load 0x0A into register 10 to invoke stdin

    ; The means by-which the IO is invoked needs to be changed.
    ; This won't be able to accuratly read in an arbitrary 'n' items :(

input_loop:
    add r0 r0 $1
    blt r0 r1 input_loop



    pop r10 ls      

    ; Do whatever


    mov r10 $0      ; Disable io checks 
>
