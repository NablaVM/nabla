.file "constants"
.init main

.int64 integer     96
.int64 integer1    42
.int64 integer2    55
.int64 integer3    568888

.double someDouble 45.435
.double lhsd       10.0
.double rhsd       1.5

.double branchTestSmall 1.0
.double branchTestLarge 5.0

.string ayyy "Hey this is a really cool string. The string limit might seem lame, but hey, maybe its okay!"


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

<arith:
    add r0 $2 $10    ; 12 
    add r0 r0  r0    ; 24
    add r1 r0  $6    ; 30
    add r0 r1  $10   ; 40
    sub r0 $10 $2    ; 8
    sub r0 r0  r0    ; 0
    sub r1 r0  $6    ; -6
    sub r0 $10 r1    ; 16
    mul r0 $2 $2     ; 4
    mul r0 r0 r0     ; 16
    mul r1 r0 $2     ; 32
    mul r1 $4 r1     ; 128
    div r0 $100 $5   ; 20
    div r0 r0 r0     ; 1
    add r0 r0 $49    ; 50
    div r1 r0 $2     ; 25
    div r1 $100 r1   ; 4
>

<modtest:
    mov r1 $10
    mov r2 $7
    call modulus     ; 3
    mov r1 $10
    mov r2 $9
    call modulus     ; 1
    mov r1 $10
    mov r2 $10
    call modulus     ; 0
    mov r1 $8
    mov r2 $10
    call modulus     ; 8
>

<powertest:
    mov r1 $2
    mov r2 $8
    call power       ; 256
>

<load_and_dbl_arith:
    ldw r1 $0(gs)   ; Should be 96
    ldw r2 $8(gs)   ; Should be 42
    ldw r3 $16(gs)  ; Should be 55
    ldw r4 $24(gs)  ; Should be 568888
    ldw r9 $0(gs)   ; load integer into 49
    add r9 r9 $1    ; add 1 to 96
    ldw r9  $40(gs)       
    ldw r10 $48(gs)      
    add.d r0 r9 r10  ; 11.5
    sub.d r0 r9 r10  ; 8.5
    mul.d r0 r10 r9  ; 15.0
    div.d r0 r9 r10  ; 6.66
>

<load_store_push_pop:
    ldw r1 $0(gs)
    stw $8(gs) r1

    ldw r1 $0(gs)

    pushw gs r1
    pushw ls r1
    popw r0 gs
    popw r0 ls
>

<jmp_test:

    mov r0 $1

    jmp gohere

    mov r0 $10 ; Should be skipped

gohere:
   

>

<main:

    ;call arith
    ;call modtest
    ;call powertest
    ;call load_and_dbl_arith
    ;call load_store_push_pop
    ;call jmp_test

; -------------------- BGT -----------------------------
;     add r0 $2 $10        ; Put 12 into reg 0
;     add r1 $5 $5         ; Put 10 into reg 1
; testLabel:
; 
;     add r9 $5 $4
;     add r1 r1 $1        ;  inc r1 by 1
; 
;     bgt r0 r1 testLabel ; while r0 > r1
; ------------------------------------------------------


; -------------------- BGTE -----------------------------
;     add r0 $2 $10        ; Put 12 into reg 0
;     add r1 $5 $5         ; Put 10 into reg 1
; testLabel:
; 
;     add r9 $5 $4
;     add r1 r1 $1        ;  inc r1 by 1
; 
;     bgte r0 r1 testLabel ; while r0 >= r1
; ------------------------------------------------------


; -------------------- BLT -----------------------------
;    add r0 $0 $0         ; Put 0 into reg 0
;    add r1 $0 $5         ; Put 5 into reg 1
;
;testLabel:
;    add r0 r0 $1        ;  inc r0 by 1
;
;    blt r0 r1 testLabel ; while r0 < r1 
; ------------------------------------------------------

; -------------------- BLTE ----------------------------
;     add r0 $0 $0         ; Put 0 into reg 0
;     add r1 $0 $5         ; Put 5 into reg 1
; 
; testLabel:
;     add r0 r0 $1        ;  inc r0 by 1
; 
;     blte r0 r1 testLabel ; while r0 < r1 
; ------------------------------------------------------


; -------------------- BEQ -----------------------------
;     add r0 $0 $0         ; Put 0 into reg 0
;     add r1 $0 $1         ; Put 1 into reg 1
; 
; testLabel:
;     add r0 r0 $1        ;  inc r0 by 1
; 
;     beq r0 r1 testLabel ; while r0 == r1 
; ------------------------------------------------------


; -------------------- BNE ----------------------------
;     add r0 $0 $0         ; Put 0 into reg 0
;     add r1 $0 $5         ; Put 5 into reg 1
; 
; testLabel:
;     add r0 r0 $1        ;  inc r0 by 1
; 
;     bne r0 r1 testLabel ; while r0 != r1 
; ------------------------------------------------------


; -------------------- BGTD ----------------------------
;    ldw r0  $56(gs)  ; small  (1.0) 
;    ldw r1  $64(gs)  ; large  (5.0)
;
;testLabel:
;    add.d r0 r0 r0  ; add 1.0 r0 
;
;    bgt.d r1 r0 testLabel
; -------------------------------------------------------


; -------------------- BGTED ----------------------------
;    ldw r0  $56(gs)  ; small  (1.0) 
;    ldw r1  $64(gs)  ; large  (5.0)
;
;testLabel:
;    add.d r0 r0 r0  ; add 1.0 r0 
;
;    bgte.d r1 r0 testLabel
; -------------------------------------------------------


; -------------------- BLTD ----------------------------
;    ldw r0  $56(gs)  ; small  (1.0) 
;    ldw r1  $64(gs)  ; large  (5.0)
;
;testLabel:
;    add.d r0 r0 r0  ; add 1.0 r0 
;
;    blt.d r0 r1 testLabel
; -------------------------------------------------------


; -------------------- BLTED ----------------------------
;    ldw r0  $56(gs)  ; small  (1.0) 
;    ldw r1  $64(gs)  ; large  (5.0)
;
;testLabel:
;    add.d r0 r0 r0  ; add 1.0 r0 
;
;    blte.d r0 r1 testLabel
; -------------------------------------------------------


; -------------------- BEQD ----------------------------
;    ldw r0  $56(gs)  ; small  (1.0) 
;    ldw r1  $64(gs)  ; large  (5.0)
;
;    add.d r0 r0 r0  ; add 1.0 r0 
;    add.d r0 r0 r0  ; add 1.0 r0 
;    add.d r0 r0 r0  ; add 1.0 r0 
;testLabel:
;    add.d r0 r0 r0  ; add 1.0 r0 
;
;    beq.d r0 r1 testLabel
; -------------------------------------------------------


; -------------------- BNEQ ----------------------------
;    ldw r0  $56(gs)  ; small  (1.0) 
;    ldw r1  $56(gs)  ; small  (1.0) 
;    ldw r2  $64(gs)  ; large  (5.0)
;
;testLabel:
;    add.d r0 r0 r1  ; add 1.0 r0 
;
;    bne.d r0 r2 testLabel
; -------------------------------------------------------

    exit
>
