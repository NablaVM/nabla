.file "ldb asm"
.init main

.string MY_STR "This is a string"
.double MY_DBL 43.22
.int64    MY_INT 45

<mov:
    mov r0 r1
    mov r3 r0
>

<stb:
    stb $10(gs) r0
    stb $99(ls) r1
>

<push:
    push gs r0
    push ls r1
>

<pop:
    pop r0 gs  
    pop r1 ls 
>

<main:

    ldb r3 $0(gs)
    ldb r3 $2(gs)
>
