.file "newoffsets"

.init  main

.int8  small 42
.int8  tiny  9
.int64 nice  69

<loads:
  ldb r0 $0(gs)  ; 42
  ldw r1 $2(gs)  ; 69
  stb $0(gs) r0  ; Stores 42 where it came from 
  stw $2(gs) r1 ; Stores 400 where it came from
  ldb r0 $0(gs)  ; 42
  ldw r1 $2(gs)  ; 69
>

<do_push:
    push  ls r0 ; Pushes LSB to ls 
    pushw ls r0 ; Pushes all of r0 to ls
    popw r3 ls  ; Pops word from ls (8 bytes)
    pop r4 ls   ; Pops single byte from ls, stores in LSB
>

<main:
    call do_push

    call loads

>