.file "Adds ASM"
.init main

.int8  m00C0234       96
.int16 EXAMPLE_INT    42
.int32 NEG_INT        -55

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

<main: 
    add r0 r1 r2         ;   Add r2 to r1 and store in r0
    add r0 $59 r2        ;   Add r2 to 59 and store in r0
    add r0 r2 $-59       ;   Add -59 to r2 and store in r0
    
    add r0 $59 $1        ;   Add r2 to 59 and store in r0
    exit
>
