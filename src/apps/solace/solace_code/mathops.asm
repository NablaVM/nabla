;
;   These are some demo math functions I'm using to see what instructionally should change.
;   Specifically testing if what I had in mind while making the instructions makes sense
;
;   Once things are settled. These mehtods can be added to a library as they are useful, however,
;   the methods could be reconfigured to use fewer registers, and they definitely should
;   practice 'callee save' where they dump the registers they will wipe out into the local stack before
;   operating, and pop them back into where they got them as-to ensure they don't muddle around with 
;   data that might be used elsewhere after the return
;

.file "MathOps"
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

;
;	MODULUS
;	Performs modulus operation on r1, and r2
;	Results in r0
;	Ex:  r0 = r1 % r2
;	Uses : r0, r1, r2, r3, r4
<modulus:

    blt r1 r2 modspecial

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
    ret 
modspecial:
    mov r0 r1
>


<main:

    ;mov r1 $2
    ;mov r2 $8
    ;call power

     mov r1 $8
     mov r2 $10
     call modulus
>