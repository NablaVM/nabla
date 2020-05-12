.file "constants"
.init main

.int8  integer     96
.int16 integer1    42
.int32 integer2    55
.int64 integer3    568888

.double someDouble 45.435
.double lhsd       10.0
.double rhsd       1.5

.double branchTestSmall 1.0
.double branchTestLarge 5.0

.string ayyy "Hey this is a really cool string. The string limit might seem lame, but hey, maybe its okay!"

<main:
    call loadstore
    call movs
    call adds
    call subs
    call muls
    call divs
    call arithmaticdbls
    call bgtdtest
    call bgtetest
    call blttest
    call bltetest
    call beqtest
    call bnetest
    call bgtdtest
    call bgtedtest
    call bltdtest
    call bltedtest
    call beqdtest
    call bneqtest

    exit
>


<loadstore:
    ldb r1 $0(gs)   ; Should be 96    
    ldb r2 $1(gs)   ; Should be 42    
    ldb r3 $2(gs)   ; Should be 55    
    ldb r4 $3(gs)   ; Should be 568888
    ldb r1 $0(gs)
    stb $10(gs) r1
    ldb r1 $0(gs)
    push gs r1
    push ls r1
    pop r0 gs
    pop r0 ls
    ret
>

<movs:
    ldb r15 $0(gs)
    mov r0 r15
    mov r1 $55
>

<adds:
    add r0 $2 $10    ; 12
    add r0 r0  r0    ; 24
    add r1 r0  $6    ; 30
    add r0 r1  $10   ; 40
    ret
>

<subs:
    sub r0 $10 $2    ; 8 
    sub r0 r0  r0    ; 0 
    sub r1 r0  $6    ; -6
    sub r0 $10 r1    ; 16
    ret
>

<muls:
    mul r0 $2 $2      ; 4  
    mul r0 r0 r0      ; 16 
    mul r1 r0 $2      ; 32 
    mul r1 $4 r1      ; 128
    ret
>

<divs:
    div r0 $100 $5     ; 20
    div r0 r0 r0       ; 1 
    add r0 r0 $49      ; 50
    div r1 r0 $2       ; 25
    div r1 $100 r1     ; 4 
    ret
>

<arithmaticdbls:
    ldb r9  $5(gs)       
    ldb r10 $6(gs)      
    add.d r0 r9 r10      ; 11.5
    sub.d r0 r9 r10      ; 8.5
    mul.d r0 r10 r9      ; 15.0
    div.d r0 r9 r10      ; 6.66
    ret
>

<bgttest:
; -------------------- BGT -----------------------------
    add r0 $2 $10        ; Put 12 into reg 0
    add r1 $5 $5         ; Put 10 into reg 1
testLabel:

    add r9 $5 $4
    add r1 r1 $1        ;  inc r1 by 1

    bgt r0 r1 testLabel ; while r0 > r1
    ret
>

<bgtetest:
; -------------------- BGTE -----------------------------
    add r0 $2 $10        ; Put 12 into reg 0
    add r1 $5 $5         ; Put 10 into reg 1
testLabel:

    add r9 $5 $4
    add r1 r1 $1        ;  inc r1 by 1

    bgte r0 r1 testLabel ; while r0 >= r1
    ret
>

<blttest:
; -------------------- BLT -----------------------------
    add r0 $0 $0         ; Put 0 into reg 0
    add r1 $0 $5         ; Put 5 into reg 1

testLabel:
    add r0 r0 $1        ;  inc r0 by 1

    blt r0 r1 testLabel ; while r0 < r1 
    ret
>

<bltetest:
; -------------------- BLTE ----------------------------
    add r0 $0 $0         ; Put 0 into reg 0
    add r1 $0 $5         ; Put 5 into reg 1

testLabel:
    add r0 r0 $1        ;  inc r0 by 1

    blte r0 r1 testLabel ; while r0 < r1 
    ret
>

<beqtest:
; -------------------- BEQ -----------------------------
    add r0 $0 $0         ; Put 0 into reg 0
    add r1 $0 $1         ; Put 1 into reg 1

testLabel:
    add r0 r0 $1        ;  inc r0 by 1

    beq r0 r1 testLabel ; while r0 == r1 
    ret
>

<bnetest:
; -------------------- BNE ----------------------------
     add r0 $0 $0         ; Put 0 into reg 0
     add r1 $0 $5         ; Put 5 into reg 1
 
 testLabel:
     add r0 r0 $1        ;  inc r0 by 1
 
     bne r0 r1 testLabel ; while r0 != r1 
    ret
>

<bgtdtest:
; -------------------- BGTD ----------------------------
    ldb r0  $7(gs)  ; small  (1.0) 
    ldb r1  $8(gs)  ; large  (5.0)

testLabel:
    add.d r0 r0 r0  ; add 1.0 r0 

    bgt.d r1 r0 testLabel
    ret
>

<bgtedtest:
; -------------------- BGTED ----------------------------
    ldb r0  $7(gs)  ; small  (1.0) 
    ldb r1  $8(gs)  ; large  (5.0)

testLabel:
    add.d r0 r0 r0  ; add 1.0 r0 

    bgte.d r1 r0 testLabel
    ret
>

<bltdtest:
; -------------------- BLTD ----------------------------
    ldb r0  $7(gs)  ; small  (1.0) 
    ldb r1  $8(gs)  ; large  (5.0)

testLabel:
    add.d r0 r0 r0  ; add 1.0 r0 

    blt.d r0 r1 testLabel
    ret
>

<bltedtest:
; -------------------- BLTED ----------------------------
    ldb r0  $7(gs)  ; small  (1.0) 
    ldb r1  $8(gs)  ; large  (5.0)

testLabel:
    add.d r0 r0 r0  ; add 1.0 r0 

    blte.d r0 r1 testLabel
    ret
>

<beqdtest:
; -------------------- BEQD ----------------------------
    ldb r0  $7(gs)  ; small  (1.0) 
    ldb r1  $8(gs)  ; large  (5.0)

    add.d r0 r0 r0  ; add 1.0 r0 
    add.d r0 r0 r0  ; add 1.0 r0 
    add.d r0 r0 r0  ; add 1.0 r0 
testLabel:
    add.d r0 r0 r0  ; add 1.0 r0 

    beq.d r0 r1 testLabel
    ret
>

<bneqtest:
; -------------------- BNEQ ----------------------------
    ldb r0  $7(gs)  ; small  (1.0) 
    ldb r1  $7(gs)  ; small  (1.0) 
    ldb r2  $8(gs)  ; large  (5.0)

testLabel:
    add.d r0 r0 r1  ; add 1.0 r0 

    bne.d r0 r2 testLabel
    ret
>