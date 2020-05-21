#
#   Create a version string
#
set(NABLA_VERSION_MAJOR 0)
set(NABLA_VERSION_MINOR 1)
set(NABLA_VERSION_PATCH 0)

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    set(NABLA_BUILD_TYPE "Release")
else()
    set(NABLA_BUILD_TYPE "Debug")
endif()

set(NABLA_VERSION_INFO "${NABLA_VERSION_MAJOR}.${NABLA_VERSION_MINOR}.${NABLA_VERSION_PATCH}-${NABLA_BUILD_TYPE}")

#
#   Add definitions so application can get the information
#
add_definitions(-DNABLA_VERSION_INFO="${NABLA_VERSION_INFO}")