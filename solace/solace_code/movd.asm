.file "movd ASM"
.init main

.double    EXAMPLE_DBL 33.33333

; THIS IS NOT YET IMPLEMENTED IN SOLACE

main: 

    movd d0 &EXAMPLE_DBL

    movd d5 d7

    movd d3 $44.4445

    exit
