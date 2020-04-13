.file "str asm"
.init main

.string MY_STR "This is a string"

<main:

    ldb r0 &MY_STR ; Copy string to register (8 bytes of it at least)

>