add_library(libien
    "src/base64.cpp"
    "src/fs_utils.cpp"
    "src/io_utils.cpp"
    "src/messagebox.cpp"
    "src/net_utils.cpp"
    "src/platform.cpp"
    "src/str_utils.cpp"
    "src/threading.cpp"
    "src/win32/windows.cpp"
)
add_library(libien::libien ALIAS libien)

if(LIBIEN_BUILD_IMAGE OR LIBIEN_BUILD_FONTS)
target_sources(libien PRIVATE 
        "src/image/image_data.cpp"
        "src/image/image.cpp"
        "src/image/image_ycbcr.cpp"
    )
    target_compile_definitions(libien PUBLIC "LIBIEN_BUILD_IMAGE")
    if(LIBIEN_BUILD_FONTS)
        target_sources(libien PRIVATE 
            "src/fonts/ttf_font.cpp"
        )
        target_compile_definitions(libien PUBLIC "LIBIEN_BUILD_FONTS")
    endif()
    
    target_link_libraries(libien PRIVATE stb)
endif()

if(MINGW)
    target_link_libraries(libien PRIVATE ws2_32)
endif()

if(LIBIEN_BUILD_SFTP)
    target_sources(libien PRIVATE
        "src/sftp/client.cpp"
    )
    target_link_libraries(libien PRIVATE libssh2_static)
    target_compile_definitions(libien PUBLIC "LIBIEN_BUILD_SFTP")
endif()

set_target_properties(libien PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_definitions(libien PRIVATE _CRT_SECURE_NO_WARNINGS)
target_include_directories(libien PUBLIC include)
target_link_libraries(libien PUBLIC fmt)