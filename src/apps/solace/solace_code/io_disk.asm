.file "diskio"
.init main


.string exampleFile "/tmp/myfile.txt"
.string prompt      "Please enter something to write to file: "


<main:

    size r0 gs
    push ls r0      ; Store the current size of the global stack locally

    call prompt_user
    call get_user_input

    pop r0 ls       ; Get the original size of the global stack into r0

    call write_user_input_to_disk
>

; --------------------------------------------------------
;           PROMPT USER FOR INPUT
; --------------------------------------------------------
<prompt_user:

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r0 r0 r1 

    size r1 gs 
    mov  r2 $2       ; Get only the second string

loop_top:

    ldb r11 r2(gs)  ; Load from gs index ( basicelly  gs[r1] )

    mov r10 r0      ; Trigger stdout

    add r2 r2 $1    ; add one to conuter 

    blt r2 r1 loop_top
>

; --------------------------------------------------------
;           GET DATA FROM USER TO WRITE TO DISK
; --------------------------------------------------------
<get_user_input:

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

; --------------------------------------------------------
;           WRITE USER INPUT TO DISK
; --------------------------------------------------------
<write_user_input_to_disk:

    ; Original stack size (before user input) is in r0

    size r2 gs          ; Get current stack size 
    sub r1 r2 r0       ; Subtract original size from current to get length of user input (in terms of stack addr)
    push r1 ls         ; Save for later


    ;
    ;  Command the IO Device to open the 'exampleFile'
    ;

    lsh r5 $10  $56    ; Load 0x0A into MSB register 5
    lsh r6 $100 $48    ; Set target to 'diskin'
    lsh r7 $1   $40    ; Set instruction to 'open'

    or r5 r5 r6        ; Assemble the command
    or r5 r5 r7        ; Assemble the command

    push ls r5         ; Store the command in ls 

    lsh r5 $0 $40      ; Load the start address of the file string 
    
    or r5 r5 $2        ; Load the end address of the file string

    mov r11 r5         ; Move the stack address information into register io device will expect
    
    pop r5 ls          ; Pop the command into r5 so we don't lose it after call
    
    mov r10 r5         ; Move command into trigger register

    beq r11 r10 bottom_label ; If fail happens r11 will be 0, and since r10 is zeroed by io device we can compare for err check


    ;
    ;  Write out the file 
    ;




bottom_label:
    ret
>