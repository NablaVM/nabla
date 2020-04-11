.file "Adds ASM"
.init main

.int m00C0234 96
.int EXAMPLE_INT 42

main: 
    add sp sp $10   ;   Increase stack by '10'

    add r0 r1 r2  ;   Add r2 to r1 and store in r0

    add r0 $59 r2  ;   Add r2 to r1 and store in r0

    add r15 r4 &m00C0234 ; Add m00C0234 to r4, store in r3

    add r4 &EXAMPLE_INT r1

    exit