.file "push_pop"
.init main

<main:

    push gs r0  ; Push r0 into global stack
    pop  r1 gs  ; Pop global stack into r0

    push ls r2 ; Push r0 into local stack
    pop  r3 ls ; Pop local stack into r0 

>
