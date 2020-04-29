.file "numbering"
.init main

.string intPrompt "Enter an integer : "

<main:

    call get_int_from_stdin_ascii

>


; ----------------------------------------------------
;                   GET INTEGER 
; ----------------------------------------------------
<get_int_from_stdin_ascii:

    ; Call stdin 
    ;  - The number of frames produced for gs will be stored in r11
    ;
    mov r10 $1 

    ; Valid integers are 48 - 57 ( 0 - 9 )

decode:

    

>