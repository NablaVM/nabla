.file "dsubs ASM"
.init main

.double EAMPLE_DBL 42.0

main: 

    dsub d0 d1 d2  ;   dsub d2 to d1 and store in d0

    dsub d0 $42.69 d2  ;   dsub d2 to d1 and store in d0

    dsub d3 d1 &EAMPLE_DBL 

    dsub d1 &EAMPLE_DBL d1
    
    exit
