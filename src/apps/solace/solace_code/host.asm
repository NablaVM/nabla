.file "host_example"

.init main

<main:

    call get_clock

    call get_clock_ps

    call epoch

    call random
>

;
;   Get clock
;
<get_clock:

    lsh r0 $12 $56    ; Load 0x0C into MSB register 0
    lsh r1 $0  $48    ; clock

    or r10 r0 r1      ; Combine into trigger register 

>

;
;   Get clocks per second
;
<get_clock_ps:

    lsh r0 $12 $56    ; Load 0x0C into MSB register 0
    lsh r1 $1  $48    ; clock PS

    or r10 r0 r1      ; Combine into trigger register 
>

;
;   Get epoch 
;
<epoch:

    lsh r0 $12 $56    ; Load 0x0C into MSB register 0
    lsh r1 $10 $48    ; epoch

    or r10 r0 r1      ; Combine into trigger register 
>

;
;   Get a random number
;
<random:

    lsh r0 $12 $56    ; Load 0x0C into MSB register 0
    lsh r1 $20 $48    ; random

    or r10 r0 r1      ; Combine into trigger register 
>