.file "coroutine"

.init main

<main:

    mov r0 $1

    call some_yield

    call some_other_method

    call some_yield
>

<some_yield:

    mov r0 $2 

    call other_yield

    yield

    mov r0 $7

    ret
>

<some_other_method:

    mov r0 $4

    call other_yield

    mov r0 $6
>

<other_yield:

    mov r0 $3

    yield 

    mov r0 $5

>