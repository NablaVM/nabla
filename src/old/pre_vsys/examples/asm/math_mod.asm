;
;	Performs modulus operation. Turn on nabla vm debug to see the steps it takes to determine result
;

.file "math_modulus"
.init main

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

    mov r1 $10
    mov r2 $3
    call modulus
>