cmake_minimum_required(VERSION 3.18)

set(libien_VERSION 0.1.0)

project(libien
    VERSION ${libien_VERSION}
    LANGUAGES C CXX
)

option(LIBIEN_BUILD_TESTS "Build tests" OFF)
option(LIBIEN_BUILD_IMAGE "Build image library" OFF)
option(LIBIEN_BUILD_SFTP "Build sftp library" OFF)
option(LIBIEN_BUILD_FONTS "Build fonts library" OFF)
option(LIBIEN_ENABLE_PLAYGROUND "Enable playground executable" OFF)
option(LIBIEN_PUBLIC_STB "Set stb library as public" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/.cache)

if(LIBIEN_BUILD_SFTP)
    CPMAddPackage(
        NAME libssh2
        GIT_REPOSITORY "https://github.com/libssh2/libssh2"
        GIT_TAG "master"
        OPTIONS 
            "BUILD_SHARED_LIBS OFF"
            "BUILD_EXAMPLES OFF"
            "BUILD_TESTING OFF"
        SYSTEM
    )
endif()

if(LIBIEN_BUILD_TESTS)
    CPMAddPackage(
        NAME catch2
        GIT_REPOSITORY "https://github.com/catchorg/Catch2"
        GIT_TAG "v3.4.0"
        OPTIONS "CATCH_INSTALL_EXTRAS OFF" "CATCH_INSTALL_DOCS OFF"
    )
endif()

add_subdirectory("third-party")
add_subdirectory("lib")

if(LIBIEN_BUILD_TESTS)
    add_subdirectory("test")
endif()

if(LIBIEN_ENABLE_PLAYGROUND)
    add_subdirectory("playground")
endif()