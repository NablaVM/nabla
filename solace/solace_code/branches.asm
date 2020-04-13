.file "branches"
.init main

.double MY_DBL 43.22
.int    MY_INT 45

<main:

; someLabel:



    bgt    r0 r1 someLabel
    bgte   r0 r1 someLabel
    blt    r0 r1 someLabel
    blte   r0 r1 someLabel
    beq    r0 r1 someLabel
    bne    r0 r1 someLabel



    bgt.d  r0 r1 someLabel
    bgte.d r0 r1 someLabel
    blt.d  r0 r1 someLabel
    blte.d r0 r1 someLabel
    beq.d  r0 r1 someLabel
    bne.d  r0 r1 someLabel
>
