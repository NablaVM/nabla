.file "mulds ASM"
.init main

.double EXAMPLE_DBL 42.0

main: 
    add $0(sp) $0(sp) $10   ;   Increase stack by '10'

    muld d0 d1 d2  ;   muld d2 to d1 and store in d0

    muld d0 $42.69 d2  ;   muld d2 to d1 and store in d0

    muld d3 d1 &EXAMPLE_DBL 

    muld d1 &EXAMPLE_DBL d1
    
    exit
