
set(NABLA_LIBC_INCLUDES
    ${CMAKE_SOURCE_DIR}/libc/stack
    ${CMAKE_SOURCE_DIR}/libc/binloader
    ${CMAKE_SOURCE_DIR}/libc/util
    ${CMAKE_SOURCE_DIR}/libc/vm/io
    ${CMAKE_SOURCE_DIR}/libc/vm/net
    ${CMAKE_SOURCE_DIR}/libc/vm
)

set(NABLA_LIBC_HEADERS
    ${CMAKE_SOURCE_DIR}/libc/stack/stack.h
    ${CMAKE_SOURCE_DIR}/libc/binloader/binloader.h
    ${CMAKE_SOURCE_DIR}/libc/util/util.h
    ${CMAKE_SOURCE_DIR}/libc/vm/io/io.h
    ${CMAKE_SOURCE_DIR}/libc/vm/net/net.h
    ${CMAKE_SOURCE_DIR}/libc/vm/vm.h
)

set(NABLA_LIBC_SOURCES
    ${CMAKE_SOURCE_DIR}/libc/stack/stack.c
    ${CMAKE_SOURCE_DIR}/libc/binloader/binloader.c
    ${CMAKE_SOURCE_DIR}/libc/util/util.c
    ${CMAKE_SOURCE_DIR}/libc/vm/io/io.c
    ${CMAKE_SOURCE_DIR}/libc/vm/net/net.c
    ${CMAKE_SOURCE_DIR}/libc/vm/vm.c
)