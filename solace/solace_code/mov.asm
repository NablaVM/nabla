.file "mov ASM"
.init main

.int    EXAMPLE_INT 42

; THIS IS NOT YET IMPLEMENTED IN SOLACE

main: 

    mov r0 &EXAMPLE_INT

    mov r5 r7

    mov r3 $44

    mov r3 sys1

    exit
