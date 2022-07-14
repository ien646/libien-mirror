add_library(libien
    "src/assert.cpp"
    "src/base64.cpp"
    "src/fs_utils.cpp"
    "src/io_utils.cpp"
    "src/net_utils.cpp"
    "src/platform.cpp"
    "src/str_utils.cpp"
)
add_library(libien::libien ALIAS libien)

if(LIBIEN_BUILD_IMAGE)
    target_sources(libien PRIVATE
        "src/image/image.cpp"
        "src/image/image_ycrcb.cpp"
    )
    target_link_libraries(libien PRIVATE stb)
endif()

if(LIBIEN_BUILD_SFTP)
    target_sources(libien PRIVATE
        "src/sftp/sftp.cpp"
    )
    target_link_libraries(libien PRIVATE libssh2)
endif()


set_target_properties(libien PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(libien PRIVATE /permissive-)
else()
    target_compile_options(libien PRIVATE -Wextra -pedantic)
endif()

target_compile_definitions(libien PRIVATE _CRT_SECURE_NO_WARNINGS)
target_include_directories(libien PUBLIC include)
target_link_libraries(libien PRIVATE fmt)