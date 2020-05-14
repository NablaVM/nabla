
set(NABLA_LIBC_INCLUDES
    ${CMAKE_SOURCE_DIR}/libc/util
    ${CMAKE_SOURCE_DIR}/libc/sockets
)

set(NABLA_LIBC_HEADERS
    ${CMAKE_SOURCE_DIR}/libc/util/util.h
    ${CMAKE_SOURCE_DIR}/libc/sockets/sockets.h
    ${CMAKE_SOURCE_DIR}/libc/sockets/sockpool.h
)

set(NABLA_LIBC_SOURCES
    ${CMAKE_SOURCE_DIR}/libc/util/util.c
    ${CMAKE_SOURCE_DIR}/libc/sockets/sockets.c
    ${CMAKE_SOURCE_DIR}/libc/sockets/sockpool.c
)