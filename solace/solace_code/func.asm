.file "funcs"
.init main


<helper:
    add r1 r1 r1

    ret
>

<main:
    add r0 r0 r1
    call helper

    ret
>