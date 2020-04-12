.file "Muls ASM"
.init main

.int EXAMPLE_INT 42

main: 

    mul r0 r1 r2  ;   mul r2 to r1 and store in r0

    mul r0 $59 r2  ;   mul r2 to r1 and store in r0

    mul r15 r4 &EXAMPLE_INT ; mul EXAMPLE_INT to r4, store in r3

    mul r4 &EXAMPLE_INT r1
    
    exit
