cmake_minimum_required(VERSION 3.8)

project(harp)

# set(HARP_DECOMPILER_SOURCES
#     ${CMAKE_CURRENT_SOURCE_DIR}/solace.cpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/bytegen.cpp
# )


add_executable(harp main.cpp ${HARP_DECOMPILER_SOURCES})

