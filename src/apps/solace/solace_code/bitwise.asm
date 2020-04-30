.file "newasm"
.init main

<main:

    mov r9  $8
    mov r7 $4
    mov r11 $1

    lsh r0 r9 $4    ; 128
    lsh r0 r9 r7   ; 128
    lsh r0 $4 r9    ; 1024
    lsh r0 $4 $8    ; 1024

    rsh r0 r9 $4       ; 0
    rsh r0 r9 r7      ; 0
    rsh r0 $1024 r9    ; 4 
    rsh r0 $1024 $8    ; 4 

    and r0 r7 $4    ; 4
    and r0 r9 r7    ; 0
    and r0 $4 r9     ; 0
    and r0 $4 $4     ; 4

    or r0 r9 $4     ; 12
    or r0 r9 r7    ; 12
    or r0 $4 r9     ; 12
    or r0 $4 $8     ; 12

    xor r0 r9 $4    ; 12
    xor r0 r9 r7   ; 12
    xor r0 $4 r9    ; 12
    xor r0 $4 $8    ; 12

    not r0 $1       ; -2
    not r0 r11      ; -2

    nop
    nop

    ret
>