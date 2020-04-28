.file "extend_load_store"

.int64 test  8675309
.int64 nl    720575940379279360     ; Encoded new line char

.init main

; Calls print on whatever is currently ready to rock
; Then prints the endline char
<println:
    mov r10 $2
    ldb r11 $1(gs)
    mov r10 $2
>

<main:

    ; Stack starts as size 2
    ldb r0 $0(gs)

    ; Push to make stack size 5 (0 -> 4)
    push gs r0
    push gs r0
    push gs r0

    ; Print '*'
    mov r11 $42 
    call println

    ; New Instructions
    call testsize       ; Test size
    call testregrefgs   ; Test register reference on global stack
    call testregrefls   ; Test register reference on local stack
>


<testsize:

    size r11 gs     ; Get the size of gs, and store in r11 (should be 5)
    size r11 ls     ; Should be 0 

    push ls r0
    push ls r0

    size r11 ls     ; Should be 2

    pop r0 ls 
    
    size r11 ls     ; Should be 1
>

<testregrefgs:

    mov r4 $0

    ldb r11 r4(gs) ; Load the 0th item from global stack (should be constant 'test')

    mov r4 $4
    mov r5 $33

    stb r4(gs) r5  ; Store 33 into gs at index 4

    ldb r11 $4(gs) ; Should get 33 from gs
>

<testregrefls:

    mov r0 $99

    ; Put a few 99s in the stack so we have something to work with
    push ls r0
    push ls r0
    push ls r0
    push ls r0

    mov r0 $2
    mov r7 $45

    stb r0(ls) r7  ; Store 45 into index 2 (spot 3) of ls
    ldb r11 r0(ls) ; Load index 2 (spot 3) into rll -> Should be 45
    ldb r11 $2(ls) ; Load index 2 (spot 3) into rll -> Should be 45

    mov r0 $3
    ldb r11 r0(ls) ; Load index 2 (spot 3) into rll -> Should be 99
    ldb r11 $3(ls) ; Should be 99
>