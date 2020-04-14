.file "Adds ASM"
.init main

.int m00C0234       96
.int EXAMPLE_INT    42
.int NEG_INT        -55

<main: 
    add r0 r1 r2         ;   Add r2 to r1 and store in r0
    add r0 $59 r2        ;   Add r2 to 59 and store in r0
    add r0 r2 $-59       ;   Add -59 to r2 and store in r0
    
    add r0 $59 $1        ;   Add r2 to 59 and store in r0
    exit
>

<muls:
    mul r0 r1 r2
    mul r0 $59 r2
    mul r0 r2 $-59 
>

<subs:
    sub r0 r1 r2
    sub r0 $59 r2
    sub r0 r2 $-59 
>

<divs:
    div r0 r1 r2
    div r0 $59 r2
    div r0 r2 $-59 
>