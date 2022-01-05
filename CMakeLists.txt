cmake_minimum_required(VERSION 3.19)

project(libien)

add_library(libien
    "src/base64.cpp"
    "src/platform.cpp"
    "src/strutils.cpp"
)

set_target_properties(libien PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(libien PUBLIC /permissive-)
else()
    target_compile_options(libien PUBLIC -Wextra -pedantic)
endif()

target_include_directories(libien PUBLIC "./include")