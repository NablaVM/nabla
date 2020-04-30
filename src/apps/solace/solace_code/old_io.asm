.file "io"

.init main

; Numerical triggers for io in io.h

.int64 hello 5216694956353126400    ; The word 'hello'
.int64 nl    720575940379279360     ; Encoded new line char

.string what "This is a string"

<echo:

    mov r10 $1      ; invoke io_stdin
    mov r1 r11      ; Copy over number of bytes read-in 
    mov r2 r12      ; Copy over number of frames produced

;
;   When put into the stack io_stidn ensures that we can pop the string out of 
;   memory and it will be in the order that it was put in. YAY
;
    mov r0 $0       ; Counter start
loop:
    add r0 r0 $1    ; Add one to counter

    pop r11 gs      ; Put the data into r11 for io_stdout 
    mov r10 $2      ; invoke io_stdout

    bne r0 r2 loop  ; Check if we've gone through all of the input
>

<stderr:

;
; Print the word 'hello' over io_stderr
;
    ldb r11 $0(gs)
    mov r10 $3

;
; Print the 'NL' char '\n' over io_stderr
;
    ldb r11 $1(gs)
    mov r10 $3

>

<main:
    ;call omgnopls

    call stderr
    call echo
>
