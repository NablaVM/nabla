if(WIN32)

    add_definitions(-DTARGET_PLATFORM_IS_WINDOWS)

    message("Platform detected :: Windows")
    message(" - Windows is not yet fully supported")

    set(NABLA_PLATFORM_STR "Windows")

elseif(UNIX AND NOT APPLE)

    add_definitions(-DTARGET_PLATFORM_IS_LINUX)

    message("Platform detected :: Linux")

    set(NABLA_PLATFORM_STR "Linux")

elseif (APPLE)

    add_definitions(-DTARGET_PLATFORM_IS_APPLE)

    message("Platform detected :: Apple")

    set(NABLA_PLATFORM_STR "Apple")

endif()


add_definitions(-DTARGET_PLATFORM_STRING="${NABLA_PLATFORM_STR}")