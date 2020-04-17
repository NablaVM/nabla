.file "branches"
.init main

.double MY_DBL 43.22
.int8    MY_INT 45

<main:

    add r0 $1 $4

someLabel:  ;

    bgt    r0 r14 someLabel
    bgte   r0 r15 someLabel
    blt    r0 r2 someLabel
    blte   r0 r3 someLabel
    beq    r0 r4 someLabel
    bne    r0 r5 someLabel



    bgt.d  r0 r6 someLabel
    bgte.d r0 r7 someLabel
    blt.d  r0 r8 someLabel
    blte.d r0 r9 someLabel
    beq.d  r0 r10 someLabel
    bne.d  r0 r11 someLabel
>
