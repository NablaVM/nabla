.file "Adds ASM"
.init main

.int m00C0234       96
.int EXAMPLE_INT    42

<main: 
    add r0 r1 r2         ;   Add r2 to r1 and store in r0
    add r0 $59 r2        ;   Add r2 to 59 and store in r0
    add r0 r2 $-59       ;   Add -59 to r2 and store in r0
    add r15 r4 &m00C0234 ;   Add m00C0234's value (not address) to r4
    add r4 &EXAMPLE_INT r1 ; Add r1 to EXAMPLE_INT's value  to r4
    
    exit
>

<muls:
    mul r0 r1 r2
    mul r0 $59 r2
    mul r0 r2 $-59 
    mul r15 r4 &m00C0234 
    mul r4 &EXAMPLE_INT r1
>

<subs:
    sub r0 r1 r2
    sub r0 $59 r2
    sub r0 r2 $-59 
    sub r15 r4 &m00C0234 
    sub r4 &EXAMPLE_INT r1
>

<divs:
    div r0 r1 r2
    div r0 $59 r2
    div r0 r2 $-59 
    div r15 r4 &m00C0234 
    div r4 &EXAMPLE_INT r1
>