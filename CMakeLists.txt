cmake_minimum_required(VERSION 3.18)

set(libien_VERSION 0.1.0)

project(libien
    VERSION ${libien_VERSION}
    LANGUAGES C CXX
)

option(LIBIEN_BUILD_TESTS "Build tests" ON)
option(LIBIEN_BUILD_IMAGE "Build image library" ON)
option(LIBIEN_BUILD_SFTP "Build sftp library" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)
set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/.cache)

if(LIBIEN_BUILD_SFTP)
    CPMAddPackage(
        NAME libssh2
        GIT_REPOSITORY "https://github.com/ien646/libssh2"
        GIT_TAG "fix-windows-macro-redefinitions"
        DOWNLOAD_ONLY YES
    )
    # Turn off libssh2's annoying warning bullshit
    set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
    file(READ ${libssh2_SOURCE_DIR}/CMakeLists.txt __LIBSSH2_CMAKELISTS)
    string(REPLACE "include(max_warnings)" "#include(max_warnings)" __LIBSSH2_CMAKELISTS ${__LIBSSH2_CMAKELISTS})
    file(WRITE ${libssh2_SOURCE_DIR}/CMakeLists.txt ${__LIBSSH2_CMAKELISTS})
    if(MSVC)
        set_directory_properties(${libssh2_SOURCE_DIR}
            APPEND PROPERTY LANGUAGE C
            APPEND PROPERTY COMPILE_OPTIONS "/wo4267" # disable size_t implicit cast warnings
            APPEND PROPERTY COMPILE_OPTIONS "/wo4068" # disable GCC pragma warnings
        )
    else()
        set_directory_properties(${libssh2_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-w")
    endif()
    add_subdirectory(${libssh2_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/libssh2)
endif()

if(LIBIEN_BUILD_TESTS)
    CPMAddPackage(
        NAME catch2
        GIT_REPOSITORY "https://github.com/catchorg/Catch2"
        GIT_TAG "v3.1.0"
        OPTIONS "CATCH_INSTALL_EXTRAS OFF" "CATCH_INSTALL_DOCS OFF"
    )
endif()

CPMAddPackage(
    NAME fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    GIT_TAG "9.0.0"
)

add_subdirectory("third-party")
add_subdirectory("lib")

if(LIBIEN_BUILD_TESTS)
    add_subdirectory("test")
endif()