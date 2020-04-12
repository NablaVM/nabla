.file "divds ASM"
.init main

.double EXAMPLE_INT 42.0

main: 

    divd d0 d1 d2  ;   divd d2 to d1 and store in d0

    divd d0 $42.69 d2  ;   divd d2 to d1 and store in d0

    divd d3 d1 &EXAMPLE_INT 

    divd d1 &EXAMPLE_INT d1
    
    exit
