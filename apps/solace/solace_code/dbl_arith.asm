.file "Add.ds ASM"
.init main

.double m00C0234       96.344 ; These need to be ldb'd to register before use
.double EXAMPLE_DBL    42.344 ; These need to be ldb'd to register before use

<main: 

    ldb r1 $0(gs)    ; Load m00C0234 into r1
    ldb r2 $1(gs)    ; Load EXAMPLE_DBL into r2

    add.d r0 r1 r2         ;   Add r2 to r1 and store in r0

    exit
>

<muls:
    mul.d r0 r1 r2
>

<subs:
    sub.d r0 r1 r2
>

<divs:
    div.d r0 r1 r2
>