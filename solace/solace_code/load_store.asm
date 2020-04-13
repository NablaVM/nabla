.file "ldb asm"
.init main

.string MY_STR "This is a string"
.double MY_DBL 43.22
.int    MY_INT 45

<main:

    ldb r0 &MY_STR
    ldb r1 &MY_INT
    ldb r2 &MY_DBL
    ldb r3 $10(gs)
    ldb r3 $2147483646(ls)
    ldb r15 r0
>

<lda:
    lda r0 &MY_STR
    lda r1 &MY_INT
    lda r2 &MY_DBL
    lda r3 $10(gs)
    lda r3 $2147483646(ls)
>

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
    pop gs r0 
    pop ls r1
>