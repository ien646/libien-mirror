cmake_minimum_required(VERSION 3.18)

set(libien_VERSION 0.1.0)

option(LIBIEN_BUILD_TESTS "Build tests" OFF)
option(LIBIEN_BUILD_IMAGE "Build image library" OFF)
option(LIBIEN_BUILD_SFTP "Build sftp library" OFF)

project(libien
    VERSION ${libien_VERSION}
    LANGUAGES C CXX
)

execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_subdirectory("third-party")
add_subdirectory("lib")

if(LIBIEN_BUILD_TESTS)
    add_subdirectory("test")
endif()