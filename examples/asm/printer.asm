.file "printer"

.init main


<main:
    
    ;   Print
    ;
    mul r0 $16 $200 ; Move has a 2^8 limit for raw numericals, so we leverage math here to get big numbs
    add r0 r0 $1
    call print_int_ascii
    call print_nl


    ;   Print
    ;
    mul r0 $432 $292 ; Move has a 2^8 limit for raw numericals, so we leverage math here to get big numbs
    mul r0 $432 $292 ; Move has a 2^8 limit for raw numericals, so we leverage math here to get big numbs
    call print_int_ascii
    call print_nl

    ;   Print
    ;
    mov r0 $0
    call print_int_ascii
    call print_nl


    ;   Print
    ;
    mov r0 $0
    sub r0 r0 $400
    call print_int_ascii
    call print_nl

    ;   Print
    ;
    mov r0 $-100
    call print_int_ascii
    call print_nl
>

;
;	MODULUS
;	Performs modulus operation on r1, and r2
;	Results in r0
;	Ex:  r0 = r1 % r2
;	Uses : r0, r1, r2, r3, r4
;   Saves: r1, r2, r3, r4
<modulus:
    
    blt r1 r2 modspecial

    pushw ls r1
    pushw ls r2 
    pushw ls r3
    pushw ls r4

	mov r3 $0   		; Init temp, and result
	div r3 r1 r2		; Divide Num/Denom
    mov r4 $0
	beq r3 r4 moduiz	; Check if mod is 0
	jmp moduinz	    	; If not, go to moduinz
moduiz:
		add r0 $0 $0	; Indicate is 0
		jmp moddone
moduinz:
		mul r3 r2 r3 	; Mult denom by result
		sub r0 r1 r3	; Sub result from num
		jmp moddone
moddone:

    popw r4 ls 
    popw r3 ls 
    popw r2 ls 
    popw r1 ls 
    ret

modspecial:
    mov r0 r1
>

; --------------------------------------------
;           Print INT - Prints integer in r0
; --------------------------------------------
<print_int_ascii:
   
    mov r1 $0

    blt r0 r1 is_neg
    jmp is_pos

is_neg:
    not r0 r0
    add r0 r0 $1
    mov r15 $1
    jmp cont

is_pos:
    mov r15 $0
    jmp cont

cont:


    mov r7 $0
    mov r8 $20

loop:
    pushw gs r0        ; Save the value they want to display in gs

    mov r1 r0 
    mov r2 $10

    call modulus      ; result in r0

    add r0 r0 $48
    pushw ls r0        ; Save the ascii of that number
    add r6 r6 $1      ; Count significant

    popw r0 gs         ; Get the rolling number they asked us to convert

    div r0 r0 $10

    add r7 r7 $1
    bne r7 r8 loop

    ;
    ;   Display
    ;
    lsh r8 $10 $56    ; Load 0x0A into MSB register 0
    lsh r9 $1  $48    ; Target stdout

    ; Make the stdout instruction seen in devices.md for stdout
    or r8 r8 r9 

    ; Required for display, set early so we can judge if negative sign needs to be displayed

    mov r0 $0

    beq r0 r15 dont_display_neg

    ; Display negative sign

    mov r11 $45
    mov r10 r8

dont_display_neg:

    mov r1 $0
    mov r2 $0         ; counter
    mov r3 $48 
    size r5 ls        ; size of stack  - ending at localStack.size()
    div r5 r5 $8      ; We popw, so we want to div size by word length to get words of stack

output_loop:

    popw r11 ls 

    bne r0 r1 display

    beq r11 r3 continue    

display:
    add r1 r1 $1
    mov r10 r8        ; Trigger stdout

continue:

    add r2 r2 $1      ; counter += 1
    
    blt r2 r5 output_loop

    bne r0 r1 done

    ; If nothing printed, print a 0

    mov r11 $48
    mov r10 r8

done:
    ret
>

; --------------------------------------------
;           Print NL
; --------------------------------------------
<print_nl:

    lsh r0 $10 $56    ; Load 0x0A into MSB register 0
    lsh r1 $1  $48    ; Target stderr

    ; Make the stderr instruction seen in devices.md for stderr
    or r0 r0 r1 

    mov r11 $10       ; 10 Happens to be the value of the NL character

    mov r10 r0        ; Trigger stderr to display new line
>

