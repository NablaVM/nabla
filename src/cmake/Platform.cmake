if(WIN32)

    add_definitions(-DTARGET_PLATFORM_IS_WINDOWS)

    message("Platform detected :: Windows")
    message(" - Windows is not yet fully supported")

elseif(UNIX AND NOT APPLE)

    add_definitions(-DTARGET_PLATFORM_IS_LINUX)

    message("Platform detected :: Linux")

elseif (APPLE)

    add_definitions(-DTARGET_PLATFORM_IS_APPLE)

    message("Platform detected :: Apple")

endif()