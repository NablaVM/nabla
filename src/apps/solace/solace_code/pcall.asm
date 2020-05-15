;
;   This is a demo / test of the pcall instruction. It stays a live for quite a while, but it will complete.
;   It spawns multple contexts and contexts from contexts to show how things can be done ' in parallel '
;

.file "Pcall"

.init main

<pcall_1:
    mov r0 $1
    mov r0 $1
    mov r0 $1
    mov r0 $1
    mov r0 $1
    pcall pcall_2   ; This will launch the functions in a new context. That context will complete once their callstack is empty
    mov r0 $1
    mov r0 $1
    mov r0 $1
>

<pcall_2:
    mov r0 $2
    mov r0 $2
    mov r0 $2
    mov r0 $2
>

<pcall_3:
    mov r0 $3
    pcall pcall_2   ; This will launch the functions in a new context. That context will complete once their callstack is empty
    mov r0 $3
>

<pcall_4:
    mov r0 $4
    pcall pcall_2   ; This will launch the functions in a new context. That context will complete once their callstack is empty
>

<main:
    pcall pcall_1   ; This will launch the functions in a new context. That context will complete once their callstack is empty
    pcall pcall_2   ; This will launch the functions in a new context. That context will complete once their callstack is empty
    pcall pcall_3   ; This will launch the functions in a new context. That context will complete once their callstack is empty
    pcall pcall_4   ; This will launch the functions in a new context. That context will complete once their callstack is empty
    mov r0 $0
    mov r0 $0
    mov r0 $0
    mov r0 $0       ; All of these instructions are placed so if debug is turned on we can see which context is executing
    mov r0 $0
    mov r0 $0
    mov r0 $0
    mov r0 $0
    mov r0 $0
    mov r0 $0
    mov r0 $0
    mov r0 $9000
    
    ; Loop for keeping context alive
    mov r1 $0
    mov r2 $2147483646
    mul r2 r2 $2
loop_top:
    add r1 r1 $1
    blt r1 r2 loop_top
>