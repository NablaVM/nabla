.file "Subs ASM"
.init main

.int EXAMPLE_INT 42

main: 

    sub r0 r1 r2  ;   sub r2 to r1 and store in r0

    sub r0 $59 r2  ;   sub r2 to r1 and store in r0

    sub r15 r4 &EXAMPLE_INT ; sub EXAMPLE_INT to r4, store in r3

    sub r4 &EXAMPLE_INT r1
    
    exit
