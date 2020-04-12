.file "dmov ASM"
.init main

.double    EXAMPLE_DBL 33.33333

; THIS IS NOT YET IMPLEMENTED IN SOLACE

main: 

    dmov d0 &EXAMPLE_DBL

    dmov d5 d7

    dmov d3 $44.4445

    exit
