.file "constplay"

.int64 nl    720575940379279360     ; Encoded new line char


.int64  strFrames 17
.string myStr "This is the string. There are many like it but this one is mine. Without me this string is nothing. Without this string, I am nothing"

.init main


<main:

    ; Need to be able to address ls and gs based off register ->   ldb r0 &r11(gs)
    ;                                                              stb &r11(ls)

    ; Maybe also a   ->    size r0 gs  
    ;                      size r0 ls ; to get the number of frames in stack



    ldb r11 $1(gs)
    mov r10 $3

    ldb r11 $2(gs)
    mov r10 $3

    ldb r11 $3(gs)
    mov r10 $3

    ldb r11 $4(gs)
    mov r10 $3

    ldb r11 $5(gs)
    mov r10 $3

    ldb r11 $6(gs)
    mov r10 $3

    ldb r11 $7(gs)
    mov r10 $3

    ldb r11 $8(gs)
    mov r10 $3
    
    ldb r11 $9(gs)
    mov r10 $3

    ldb r11 $10(gs)
    mov r10 $3

    ldb r11 $11(gs)
    mov r10 $3

    ldb r11 $12(gs)
    mov r10 $3

    ldb r11 $13(gs)
    mov r10 $3

    ldb r11 $14(gs)
    mov r10 $3

    ldb r11 $15(gs)
    mov r10 $3

    ldb r11 $16(gs)
    mov r10 $3

    ldb r11 $17(gs)
    mov r10 $3
    
    ldb r11 $18(gs)
    mov r10 $3

; Print NL
    ldb r11 $0(gs)
    mov r10 $3
>