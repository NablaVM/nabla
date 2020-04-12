.file "subds ASM"
.init main

.double EAMPLE_DBL 42.0

main: 

    subd d0 d1 d2  ;   subd d2 to d1 and store in d0

    subd d0 $42.69 d2  ;   subd d2 to d1 and store in d0

    subd d3 d1 &EAMPLE_DBL 

    subd d1 &EAMPLE_DBL d1
    
    exit
