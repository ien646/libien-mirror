if(MSVC)
    add_compile_options(/W0)
else()
    add_compile_options(-w)
endif()

add_subdirectory(fmt)

if(LIBIEN_BUILD_TESTS)
    add_subdirectory(catch2)
endif()

if(LIBIEN_BUILD_SFTP)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    add_subdirectory(libssh2)
endif()

if(LIBIEN_BUILD_IMAGE)
    add_subdirectory(stb)
endif()