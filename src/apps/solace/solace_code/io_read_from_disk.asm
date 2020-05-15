;
;       Prompts the user for a file name. Reads file into memory if it exists
;       Displays error message to user if it doesn't exist.
;       Displays loaded contents if it can load the file
;


.file "ReadDisk"

.init main


.string prompt      "Enter a file to read in: "
.string error       "Could not open file."

<main:

    size r0 gs
    pushw ls r0      ; Store the current size of the global stack locally

    call prompt_user
    call get_user_input

    popw r0 ls       ; Get the original size of the global stack into r0

    size r1 gs      ; Store new size  
    pushw ls r1

    call read_from_disk

    popw r0 ls       ; popw previous size of gs
    call display_loaded_contents
>


; --------------------------------------------------------
;           PROMPT USER FOR INPUT
; --------------------------------------------------------
<prompt_user:

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r0 r0 r1 

    mov r1 $25 
    mov r2 $0

loop_top:

    ldb r11 r2(gs)  ; Load from gs index ( basicelly  gs[r1] )

    mov r10 r0      ; Trigger stdout

    add r2 r2 $1    ; add one to conuter 

    blt r2 r1 loop_top
>

; --------------------------------------------------------
;           DISPLAY ERROR TO USER
; --------------------------------------------------------
<error_to_user:

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r0 r0 r1 

    mov r1 $45
    mov r2 $25

loop_top:

    ldb r11 r2(gs)  ; Load from gs index ( basicelly  gs[r2] )

    mov r10 r0      ; Trigger stdout

    add r2 r2 $1    ; add one to conuter 

    blt r2 r1 loop_top

    ; Print and endline - NL happens to be 10 in dec
    mov r11 $10
    mov r10 r0
>

; --------------------------------------------------------
;           GET FILE NAME FROM USER
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
;            READ FROM DISK
; --------------------------------------------------------
<read_from_disk:

    lsh r5 $10  $56    ; Load 0x0A into MSB register 5
    lsh r6 $100 $48    ; Set target to 'diskin'
    lsh r7 $1   $40    ; Set instruction to 'open'

    or r5 r5 r6        ; Assemble the command
    or r5 r5 r7        ; Assemble the command

    pushw ls r5         ; Store the command in ls 

    size r1 gs

    lsh r5 r0 $32      ; Load the start address of the file string 

    or r5 r5 r1        ; Load the end address of the file string

    mov r11 r5         ; Move the stack address information into register io device will expect

    popw r5 ls          ; popw the command into r5 so we don't lose it after call
    
    mov r10 r5         ; Move command into trigger register

    beq r11 r10 error_label 

    ; File should be open now

    lsh r5 $10  $56    ; Load 0x0A into MSB register 5
    lsh r6 $100 $48    ; Set target to 'diskin'
    lsh r7 $10  $40    ; Set instruction to 'read'
    lsh r8 $100 $8     ; Number of bytes to read
    
    or r5 r5 r6        ; Assemble the command
    or r5 r5 r7        ; Assemble the command
    or r5 r5 r8

    mov r1 $0          ; Load 0 for checking if bytes read-in == 0

read_file_loop:

    mov r10 r5         ; Ask to read a single byte

    bne r1 r11 read_file_loop ; We read until bytes read-in is 0

    ;
    ;   Close the file
    ;
    
    lsh r5 $10  $56    ; Load 0x0A into MSB register 5
    lsh r6 $200 $48    ; Set target to 'close'

    or r10 r5 r6        ; Assemble the command into trigger register

    ; Bottom of good execution
    ret

error_label:
    call error_to_user
    exit
>

; --------------------------------------------------------
;            DISPLAY LOADED CONTENTS
; --------------------------------------------------------

<display_loaded_contents:

    mov r9 r0 ; Move the original stack size to r9

    ; Print whatever was read in

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r0 r0 r1 

    mov r2 r9         ; counter - starting at globalStack[fileInputIdx]

    size r5 gs        ; size of stack  - ending at globalStack.size()

output_loop:

    ldb r11 r2(gs)

    mov r10 r0        ; Trigger stdout

    add r2 r2 $1      ; counter += 1
    
    bne r2 r5 output_loop
>