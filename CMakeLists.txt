cmake_minimum_required(VERSION 3.18)

set(libien_VERSION 0.1.0)

project(libien
    VERSION ${libien_VERSION}
    LANGUAGES C CXX
)

option(LIBIEN_BUILD_TESTS "Build tests" OFF)
option(LIBIEN_BUILD_IMAGE "Build image library" OFF)
option(LIBIEN_BUILD_SFTP "Build sftp library" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/.cache)

if(LIBIEN_BUILD_SFTP)
    CPMAddPackage(
        NAME libssh2
        GIT_REPOSITORY "https://github.com/ien646/libssh2"
        GIT_TAG "libssh2-1.11.0__FIX_CMAKE"
        OPTIONS 
            "BUILD_SHARED_LIBS OFF"
            "BUILD_EXAMPLES OFF"
            "BUILD_TESTING OFF"
        SYSTEM
    )
endif()

if(LIBIEN_BUILD_TESTS)
    enable_testing()
    CPMAddPackage(
        NAME catch2
        GIT_REPOSITORY "https://github.com/catchorg/Catch2"
        GIT_TAG "v3.3.2"
        OPTIONS "CATCH_INSTALL_EXTRAS OFF" "CATCH_INSTALL_DOCS OFF"
    )
endif()

CPMAddPackage(
    NAME fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    GIT_TAG "9.1.0"
)

add_subdirectory("third-party")
add_subdirectory("lib")

if(LIBIEN_BUILD_TESTS)
    add_subdirectory("test")
endif()