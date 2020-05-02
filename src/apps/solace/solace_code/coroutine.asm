.file "coroutine"

.init main

<main:

    mov r0 $33
    mov r1 $22

    call save_registers

    call some_other_method

    call save_registers
>


<save_registers:

    push ls r0 
    push ls r1 
    push ls r2 
    push ls r3 
    push ls r4 
    push ls r5 
    push ls r6 
    push ls r7 
    push ls r8
    push ls r9 

    yield

    pop r9 ls 
    pop r8 ls 
    pop r7 ls 
    pop r6 ls 
    pop r5 ls 
    pop r4 ls 
    pop r3 ls 
    pop r2 ls 
    pop r1 ls
    pop r0 ls 

    ret
>

<some_other_method:

    mov r0 $33
    mov r1 $22

    add r3 r0 r1 
    add r4 r2 $21
>