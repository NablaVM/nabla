.file "funcs"
.init main

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

    add r9 $400 $20
>