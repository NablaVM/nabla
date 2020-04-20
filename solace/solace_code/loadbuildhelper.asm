.file "constants"
.init main

.int8  integer     96
.int16 integer1    42
.int32 integer2    55
.int64 integer3    568888

.double someDouble 45.435
.double lhsd       10.0
.double rhsd       1.5
.string ayyy "Hey this is a really cool string. The string limit might seem lame, but hey, maybe its okay!"

<dummy:
    exit
>

<another:
    exit
>

<main:

; ----------------------- Working -----------------------

    ; ldb r1 $0(gs) ; Should be 96
    ; ldb r2 $1(gs) ; Should be 42
    ; ldb r3 $2(gs) ; Should be 55
    ; ldb r4 $3(gs) ; Should be 568888


    ; ldb r9 $0(gs)       ; load integer into 49
    ; add r9 r9 $1        ; add 1 to 96

    ; add r0 $2 $10    ; 12 
    ; add r0 r0  r0    ; 24
    ; add r1 r0  $6    ; 30
    ; add r0 r1  $10   ; 40

    ; sub r0 $10 $2    ; 8
    ; sub r0 r0  r0    ; 0
    ; sub r1 r0  $6    ; -6
    ; sub r0 $10 r1    ; 16

    ; mul r0 $2 $2       ; 4
    ; mul r0 r0 r0       ; 16
    ; mul r1 r0 $2       ; 32
    ; mul r1 $4 r1       ; 128

    ; div r0 $100 $5     ; 20
    ; div r0 r0 r0       ; 1
    ; add r0 r0 $49      ; 50
    ; div r1 r0 $2       ; 25
    ; div r1 $100 r1     ; 4

; ----------------------- In progress -----------------------

    ldb r9  $5(gs)       ;lhsd    ; These dont work yet
    ldb r10 $6(gs)       ;rhsd    ; These dont work yet
    add.d r0 r9 r10      ;        ; These dont work yet


    exit
>
