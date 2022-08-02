cmake_minimum_required(VERSION 3.18)

set(libien_VERSION 0.1.0)

option(LIBIEN_BUILD_TESTS "Build tests" OFF)
option(LIBIEN_BUILD_IMAGE "Build image library" OFF)
option(LIBIEN_BUILD_SFTP "Build sftp library" OFF)

include(cmake/CPM.cmake)

if(LIBIEN_BUILD_SFTP)
    CPMAddPackage(
        NAME libssh2
        GIT_REPOSITORY "https://github.com/libssh2/libssh2"
        GIT_TAG "libssh2-1.10.0"
        OPTIONS "BUILD_EXAMPLES OFF" "BUILD_TESTING OFF"
    )
    # check if SYSTEM support is added to CPM
    get_target_property(_LIBSSH2_INCLUDES libssh2 INCLUDE_DIRECTORIES)
    target_include_directories(libssh2 SYSTEM INTERFACE ${_LIBHSS2_INCLUDES})
endif()

if(LIBIEN_BUILD_TESTS)
    CPMAddPackage(
        NAME catch2
        GIT_REPOSITORY "https://github.com/catchorg/Catch2"
        GIT_TAG "v3.1.0"
    )
endif()

CPMAddPackage(
    NAME fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    GIT_TAG "9.0.0"
)

project(libien
    VERSION ${libien_VERSION}
    LANGUAGES C CXX
)

add_subdirectory("third-party")
add_subdirectory("lib")

if(LIBIEN_BUILD_TESTS)
    add_subdirectory("test")
endif()