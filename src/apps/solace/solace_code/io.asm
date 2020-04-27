.file "io"

.init main

; Numerical triggers for io in io.h

<input:

; ----------------------
;    mov r10 $10       ; Read a single character
;    pop r0 gs         ; Remove the read-in character from the global stack
; ----------------------


; --------------------------------
;     mov r11 $10        ; Indicate to the soon-to-be-called read that we want 10 characters
;     mov r10 $11        ; Invoke ip to read r11-number of characters
; 
;     ; loop through and pop all chars
; 
;     mov r0 $0
; loop:
;     add r0 r0 $1
;     pop r1 gs
;     bne r0 r11 loop
; ---------------------------


; ----------------------------------
;    mov r10 $12     ; Indicate to read in integer
;    pop r0 gs
; ----------------------------------


; ----------------------------------
;    mov r10 $13     ; Indicate to read double
;    pop r0 gs
; ----------------------------------


>

<main:


    call input
>
