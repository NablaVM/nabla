.file "divs ASM"
.init main

.int EXAMPLE_INT 42

main: 

    div r0 r1 r2  ;   div r2 to r1 and store in r0

    div r0 $59 r2  ;   div r2 to r1 and store in r0

    div r15 r4 &EXAMPLE_INT ; div EXAMPLE_INT to r4, store in r3

    div r4 &EXAMPLE_INT r1
    
    exit
