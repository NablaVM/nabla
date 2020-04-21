.file "funcs"
.init main

.int8 FIX_THIS 0 ; Apparently no constants causes a crash =/

<heyy:
    add r0 r0 $1

    ret
>

<tester:
    add r0 r0 $1

    call heyy
>

<test:
    add r0 r0 $1

    call tester

    ret
>

<helper:
    add r0 r0 $1

    call test
>

<main:
    add r0 $0 $1

    call helper

    add r10 $400 $20
>