.file "jump"
.init main

<main:

    add r0 $0 $1
    add r1 $0 $10

someLabel:

    add r0 r0 $1
    beq r0 r1 endLabel

    jmp someLabel;

endLabel:
    exit
>
