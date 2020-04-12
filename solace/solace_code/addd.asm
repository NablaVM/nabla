.file "addds ASM"
.init main

.double EXAMPLE_DBL 42.0

main: 

    addd d0 d1 d2  ;   addd d2 to d1 and store in d0

    addd d0 $42.69 d2  ;   addd d2 to d1 and store in d0

    addd d3 d1 &EXAMPLE_DBL 

    addd d1 &EXAMPLE_DBL d1
    
    exit
