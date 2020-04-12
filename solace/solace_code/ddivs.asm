.file "ddivs ASM"
.init main

.double EXAMPLE_INT 42.0

main: 

    ddiv d0 d1 d2  ;   ddiv d2 to d1 and store in d0

    ddiv d0 $42.69 d2  ;   ddiv d2 to d1 and store in d0

    ddiv d3 d1 &EXAMPLE_INT 

    ddiv d1 &EXAMPLE_INT d1
    
    exit
