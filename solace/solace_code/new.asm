.file "test drive"
.init main

<main:

    call other 

    pop r0 gs ; puts 33 in r0
    pop r1 gs ; puts 42 in r1

    ; gs is now empty

    push ls r0  ; store 33 in local stack
    push ls r1  ; store 42 in local stack

    ldb r3 $1(ls) ; Loads 33 from local stack 
    stb $0(ls) $55 ; Overwrites 42 in ls to 55

    pop r0 ls ; 55 now in r0

    pop r1 ls ; 33 now in r3
>


<other:

    push gs $42
    push gs $33

>