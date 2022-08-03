cmake_minimum_required(VERSION 3.18)

set(libien_VERSION 0.1.0)

option(LIBIEN_BUILD_TESTS "Build tests" OFF)
option(LIBIEN_BUILD_IMAGE "Build image library" OFF)
option(LIBIEN_BUILD_SFTP "Build sftp library" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

if(LIBIEN_BUILD_SFTP)
    CPMAddPackage(
        NAME libssh2
        GIT_REPOSITORY "https://github.com/libssh2/libssh2"
        GIT_TAG "libssh2-1.10.0"
        DOWNLOAD_ONLY YES
    )
    # Turn off libssh2's annoying warning bullshit
    set(BUILD_EXAMPLES OFF)
    set(BUILD_TESTING OFF)
    file(READ ${libssh2_SOURCE_DIR}/CMakeLists.txt __LIBSSH2_CMAKELISTS)
    string(REPLACE "include(max_warnings)" "#include(max_warnings)" __LIBSSH2_CMAKELISTS ${__LIBSSH2_CMAKELISTS})
    file(WRITE ${libssh2_SOURCE_DIR}/CMakeLists.txt ${__LIBSSH2_CMAKELISTS})
    set_directory_properties(${libssh2_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "/W0")
    add_subdirectory(${libssh2_SOURCE_DIR})
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