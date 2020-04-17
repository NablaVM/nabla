.file "str asm"
.init main

.string MY_STR "This is a string"

<main:

    ldb r0 $0(gs) ; Load 8 bytes of string into r0

>