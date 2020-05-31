
#
#   NHLL
#
set(NHLL_COMPILER_DIR
    ${NABLA_COMPILERS_DIR}/nhll
)

include(${NHLL_COMPILER_DIR}/nhll.cmake)


#
#   Build the includes
#
set(NABLA_COMPILERS_INCLUDE_DIRS
    ${NABLA_COMPILERS_DIR}
    ${NHLL_COMPILER_DIR}
    ${NHLL_INCLUDE_DIRS}
)

#
#   Add sources for all compilers 
#
set(NABLA_COMPILERS_SOURCES
    ${NHLL_COMPILER_SOURCES}
)