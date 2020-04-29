.file "pritnstring"

.int64 nl    720575940379279360     ; Encoded new line char

.string myStr "This is the string. There are many like it but this one is mine. Without me this string is nothing. Without this string, I am nothing"

.init main

<main:

    size r0 gs      ; Get the size of the string
    mov r1 $1       ; Ensure 1 is in the register so we can use it as a counter (start at 1 to skip nl)

loop:
    ldb r11 r1(gs)  ; Load a segment of the string from the global stack
    mov r10 $2      ; Call stdout

    add r1 r1 $1    ; Add one

    blt r1 r0 loop  ; Check if we've hit the end of the string

    ldb r11 $0(gs)  ; Load new line
    mov r10 $2      ; Call stdout
>