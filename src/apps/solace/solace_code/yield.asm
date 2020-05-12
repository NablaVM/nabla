.file "yield"

.init main

<main:
    call f 
    mov r0 $99
    call f
    call f
>


<f:

    mov r0 $1

    yield

    mov r0 $2
>