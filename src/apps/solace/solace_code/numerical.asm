
.init main

.int8  a -120
.int64 b  9223372036854775807
.int64 c -9223372036854775807

<main:

    ; Mov is limited to an 8-bit int 
    mov r0 $-1

    add r0 $0 $-10

    sub r0 $-20000 $11000

    add r0 r0 $30000
>