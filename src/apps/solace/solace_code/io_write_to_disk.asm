.file "diskio"
.init main


.string exampleFile "/tmp/myfile.txt"
.string prompt      "Please enter something to write to file: "


<main:

    size r0 gs
    pushw ls r0      ; Store the current size of the global stack locally

    call prompt_user
    call get_user_input

    popw r0 ls       ; Get the original size of the global stack into r0

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
    mov  r2 $15       ; Get only the second string

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

    ; Original global stack size (before user input) is in r0
    pushw ls r0         


    ;
    ;  Command the IO Device to open the 'exampleFile'
    ;

    lsh r5 $10  $56    ; Load 0x0A into MSB register 5
    lsh r6 $101 $48    ; Set target to 'diskout'
    lsh r7 $1   $40    ; Set instruction to 'open'
    lsh r8 $1   $32    ; Set the mode to 'write / create'

    or r5 r5 r6        ; Assemble the command
    or r5 r5 r7        ; Assemble the command
    or r5 r5 r8

    pushw ls r5         ; Store the command in ls 

    lsh r5 $0 $32      ; Load the start address of the file string 
    
    or r5 r5 $15        ; Load the end address of the file string

    mov r11 r5         ; Move the stack address information into register io device will expect
    
    popw r5 ls          ; popw the command into r5 so we don't lose it after call
    
    mov r10 r5         ; Move command into trigger register

    beq r11 r10 bottom_label ; If fail happens r11 will be 0, and since r10 is zeroed by io device we can compare for err check

    ;
    ;  Write out the file 
    ;

    lsh r5 $10  $56    ; Load 0x0A into MSB register 5
    lsh r6 $101 $48    ; Set target to 'diskout'
    lsh r7 $10  $40    ; Set instruction to 'write'

    or r5 r5 r6        ; Assemble the command
    or r5 r5 r7        ; Assemble the command

    mov r0 r5          ; store in r0


    popw r2 ls          ; popw the original stack size into r2 for counter globalStack[userInputIdx]

    size r5 gs

output_loop:

    ldb r11 r2(gs)    

    mov r10 r0        ; Trigger disk write

    add r2 r2 $1      ; counter += 1

    bne r2 r5 output_loop


    ;
    ;   Close the file
    ;
    
    lsh r5 $10  $56    ; Load 0x0A into MSB register 5
    lsh r6 $200 $48    ; Set target to 'close'

    or r10 r5 r6        ; Assemble the command into trigger register

bottom_label:
    ret
>