.file "dadds ASM"
.init main

.double EXAMPLE_DBL 42.0

main: 

    dadd d0 d1 d2  ;   dadd d2 to d1 and store in d0

    dadd d0 $42.69 d2  ;   dadd d2 to d1 and store in d0

    dadd d3 d1 &EXAMPLE_DBL 

    dadd d1 &EXAMPLE_DBL d1
    
    exit
