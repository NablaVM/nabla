
#
#   Create a version string
#
set(NABLA_VERSION_MAJOR 0)
set(NABLA_VERSION_MINOR 1)
set(NABLA_VERSION_PATCH 0)

set(NABLA_VERSION_INFO "${NABLA_VERSION_MAJOR}.${NABLA_VERSION_MINOR}.${NABLA_VERSION_PATCH}")

#
#   Generate a timestamp
#
string(TIMESTAMP NABLA_COMPILATION_TIMESTAMP "%a %Y-%B-%d %H:%M:%S")

#
#   Generate a build hash
#
string(SHA3_512 FULL_TIME_HASH ${NABLA_COMPILATION_TIMESTAMP})
string(SHA3_512 FULL_VERS_HASH ${NABLA_VERSION_INFO})

string(SUBSTRING ${FULL_VERS_HASH} 0  5 NABLA_VERS_HASH)
string(SUBSTRING ${FULL_TIME_HASH} 0 10 NABLA_TIME_HASH)

set(NABLA_BUILD_ID_SHORT "${NABLA_VERS_HASH}-${NABLA_TIME_HASH}")
set(NABLA_BUILD_ID_LONG  "${FULL_VERS_HASH}-${FULL_TIME_HASH}")

#
#   Show the information
#
message("Version: ${NABLA_VERSION_INFO}")

message("Compilation Timestamp: ${NABLA_COMPILATION_TIMESTAMP}")

message("Short build id: ${NABLA_BUILD_ID_SHORT}")

# We don't need to show this
#
# message("Long build id :\n ${NABLA_BUILD_ID_LONG}")

#
#   Add definitions so application can get the information
#
add_definitions(-DNABLA_VERSION_INFO="${NABLA_VERSION_INFO}")

add_definitions(-DNABLA_COMPILATION_TIMESTAMP="${NABLA_COMPILATION_TIMESTAMP}")

add_definitions(-DNABLA_BUILD_ID_SHORT="${NABLA_BUILD_ID_SHORT}")