This is just a brief overview of the language. There is more to the language than what is listed here. 

To see a full listing of instructions, [look over here.](https://github.com/bosley/nabla/wiki/Nabla-Byte-Code) 

## Functions

All instructions must be placed in a function. When a function is accessed it has an empty 'local stack' that can only be used by that function. When the function is exited (by hitting end of function, or 'ret'), the local stack is freed. 

```asm
.file "Some file name info"
.init main

; The 'init' function - Where the VM will start off
<main:
    ; Instructions for the function go here.
    ; When a function is accessed, its local stack is created and of size '0' 

    ; Call the function 'example' Functions can be define in whatever order,
    ; as long as they get defined if solace is being used to compile the byte code
    call example

    ; While not required, ret should be used for efficiency sake. More steps are required to attempt
    ; returning if not explicitly told to return 
    ret
>

<example:
    ; Example now has its own local stack to operate with, and can access the global stack / registers
    ret 
>
```

## Constants 

Constants in ASM are predefined values to be loaded into the program's global stack in the order that they are defined. 
Not all constants are shown here. Check out the [Nable Byte Code](https://github.com/bosley/nabla/wiki/Nabla-Byte-Code) document to see all of the constant directives. 

```asm

.double pi 3.14159    ; A double. 
.int64  answer 42     ; An 8-byte integer. 

.string myStr  "This is an example of a string." 

.init main

<main:

    ret
>

```

## Loading and storing

```asm

; Anywhere that 'ls' is seen, 'gs' can be placed to work with the global stack instead of the local stack. 

.init main

<main:
    mov r9 $45 ; Put the number '45' into register 9
    mov r7 r9  ; Put value of r9 into r7 

    pushw ls r9 ; Push the value from r9 into the local stack

    size r0 ls ; Get the number of frames in the local stack and store it in r0 (Should be 1 at this time)

    popw r10 ls ; Pop the new value into a new register

    pushw ls r9 ; Put r9 back into the local stack

    ; The next two instructions need to be done with care. If the stacks are indexed out of bounds, a fault will occur

    ldb r0 $0(ls) ; Load the value placed in without popping

    stw $0(ls) r9 ; Place the value of a register into a stack.

    ; In addition to using $N(ls), the stacks can be indexed by a register value for stw and ldw. 

    mov r0 $1

    ldw r1 r0(ls) ; Loads local stack value at the index of the value in r0 (1)
    stw r0(ls) r9 ; The same thing works with stw

    ret
>
```

## Jumping Around

```asm

.init main

; Jumping can only occur within a function. You can not jump to labels in other functions
; Labels jumped to can be before or after the jmp instruction

<main:

    jmp first

second:             ; Labels look a lot like function declarations, but they aren't instructions at all. 
    jmp last        ; when solace runs over the .asm file, they are turned into addresses for placement 
                    ; inside the jmp instruction
first:
    jmp second

last:
    ret
>

```

## Branching 

```asm

; There are a lot of branches, so we aren't showing them all here. No way.
; Branches branch to labels, and can only branch within their function. 
; To branch based of registers containing doubles, the branch '.d' variant must be used, 
; otherwise weird 'undefined' things can happen

.init main

<main:
    mov r0 $0    ; Put 0 into reg 0
    mov r1 $5    ; Put 5 into reg 1

testLabel:
    add r0 r0 $1        ;  inc r0 by 1

    blt r0 r1 testLabel ; while r0 < r1
    ret
>
```

## Getting out

```asm
.init main

<awesome:
    exit    ; The moment exit is found, the vm bails out. No further processing will occur
>

<moreso:
    ret
>

<main:
    call awesome
    call moreso    ; This won't happen at all 
    ret
>
```

## The meat and potatoes - Math

```asm
; For every arithmetic operation listed, there is a '.d' variant for
; doing that same operation on doubles. Note: .d operations don'e allow
; numerical drop-ins (things prefixed with a '$' 

.init math

<math:
    mov r0 $2

    mul r0 r0 $2  ; Multiply 2 by 2 and store in r0
    add r0 r0 r0  ; Add 4 to 4 and store in r0
    sub r0 r0 $2  ; Subtract 2 from 4 store in r0
    div r0 $10 r0 ; Divide 10 by 2 and store in r0
>

```



