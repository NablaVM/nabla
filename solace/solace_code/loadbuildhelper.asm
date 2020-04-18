.file "constants"
.init main

.int8  integer     96
.int16 integer1    42
.int32 integer2    55
.int64 integer3    568888

.double someDouble 45.435

.string ayyy "Hey this is a really cool string. The string limit might seem lame, but hey, maybe its okay!"

<dummy:
    exit
>

<another:
    exit
>

<main:

    ldb r1 $0(gs) ; Should be 96
    ldb r2 $1(gs) ; Should be 42
    ldb r3 $2(gs) ; Should be 55
    ldb r4 $3(gs) ; Should be 568888

    exit
>
