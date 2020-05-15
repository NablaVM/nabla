;
;   This is to test the preprocessing of labels
;   Labels must only be unique to the function. 
;   When running, the value 800 should never be seen
;

.file "branchtest"
.init main


<main:

    add r0 $1 $0
    add r1 $0 $0

prelabel:           ; Should occur once
    add r1 r1 $1

    beq r0 r1 prelabel

    bne r0 r1 postlabel

    add r13 $400 $400 ; This shouldn't happen

postlabel:

    add r0 $40 $2

    call sep        ; Because we can call functions anywhere as long as they exist now

    exit
>


<sep:
    add r0 $1 $0
    add r1 $0 $0

prelabel:           ; Should occur once
    add r1 r1 $1

    beq r0 r1 prelabel

    bne r0 r1 postlabel

    add r13 $400 $400 ; This shouldn't happen

postlabel:

    add r0 $40 $2

    ret
>
