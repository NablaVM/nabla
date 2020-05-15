.file "newmov"
.init main
<main:

    mov r0 $-2147483647  ; Absolute Limit (as defined by numerical_limit<int32_t>)
    mov r0 $2147483646   ; Absolute Limit (as defined by numerical_limit<int32_t>)
>