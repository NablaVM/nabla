cmake_minimum_required(VERSION 3.8)

project(nablaLang)

# -------------------------------------------------
#   Solace Parser
# -------------------------------------------------

add_subdirectory(apps/solace)

# -------------------------------------------------
#   Harp decompiler
# -------------------------------------------------

add_subdirectory(apps/harp)

# -------------------------------------------------
#   Virtual Machine
# -------------------------------------------------

add_subdirectory(apps/vm)
