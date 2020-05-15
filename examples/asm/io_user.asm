.file "io"
.init main

.string prompt "Please enter some text : " 


;   STDIN Command structure 
;
;   ID         TARGET    [ ---------- NUM BYTES TO READ ------------ ]   [ TERM  ]   [ UNUSED ]
; 0000 1010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

;   STDOUT Command structure
;
;    ID         TARGET    [ --------------------------- UNUSED -------------------------------]
; 0000 1010 | 0000 0001 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 

;   STDERR Command structure
;
;    ID         TARGET    [ --------------------------- UNUSED -------------------------------]
; 0000 1010 | 0000 0010 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 


; -----------------------------------------
;                   MAIN
; -----------------------------------------
<main:

    call display_prompt         ; Display the constant string for a prompt

    size r0 gs 
    push ls r0

    call echo_invoke_stdin      ; Invoke stdin to get some user input
    
    pop r0 ls
    
    call echo_invoke_stdout     ; Invoke stdout to give the data back to the user
    call nl_with_stderr         ; Print a new line using stderr to show how to use it
>

; -----------------------------------------
;            DISPLAY PROMPT
; -----------------------------------------
<display_prompt:

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r0 r0 r1 

    size r1 gs 
    mov  r2 $0

loop_top:

    ldb r11 r2(gs)  ; Load from gs index ( basically  gs[r1] )

    mov r10 r0      ; Trigger stdout

    add r2 r2 $1    ; add one to counter 

    blt r2 r1 loop_top
>

; -----------------------------------------
;             STDIN FOR ECHO
; -----------------------------------------
<echo_invoke_stdin:

    ; This could be done better, but being verbose for testing

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $0  $48    ; Not really neaded with stdin 
    lsh r2 $255 $16   ; Read up-to 255 chars
    lsh r3 $10 $8     ; Load the terminating char '\n' 

    ; Make the stdin instruction seen in devices.md for stdin

    or r0 r0 r1       ; Pack into single register
    or r0 r0 r2       ; Pack into single register
    or r0 r0 r3       ; Pack into single register

    mov r10 r0        ; Move the command into the trigger register 
>

; -----------------------------------------
;           STDOUT FOR ECHO
; -----------------------------------------
<echo_invoke_stdout:

    mov r9 r0 ; Move the original stack size to r9

    ; Print whatever was read in

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r0 r0 r1 

    mov r2 r9         ; counter - starting at globalStack[userInputIdx]

    size r5 gs        ; size of stack  - ending at globalStack.size()

output_loop:

    ldb r11 r2(gs)

    mov r10 r0        ; Trigger stdout

    add r2 r2 $1      ; counter += 1
    
    bne r2 r5 output_loop
>

; -----------------------------------------
;         STDERR FOR NEW LINE 
; -----------------------------------------
<nl_with_stderr:

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $2  $48    ; Target stderr

    ; Make the stderr instruction seen in devices.md for stderr
    or r0 r0 r1 

    mov r11 $10       ; 10 Happens to be the value of the NL character

    mov r10 r0        ; Trigger stderr to display new line
>