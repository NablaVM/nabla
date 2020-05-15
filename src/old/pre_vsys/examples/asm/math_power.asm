;
;   Raises a number to the power of a different number
;


.file "math_power"
.init main

;
;      POWER
;      Performs power operation on r1, and r2
;      Results in r0
;      Ex:  r0 = r1 ^ r2
;      Uses : r0, r1, r2, r3
<power:
        mov r0 r1
        mov r3 $1                   ; Start a counter 
        bgte r3 r2 powerbottom      ; Pre-Test
powertop:
        mul r0 r0 r1			    ; Multiply the and sum
        add r3 r3 $1
        blt r3 r2 powertop
powerbottom:
>

<main:
    mov r1 $2
    mov r2 $8
    call power
>