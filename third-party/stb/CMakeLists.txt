add_library(stb src/stb.cpp)

target_include_directories(stb PUBLIC include)

if(LIBIEN_BUILD_IMAGE)
    target_compile_definitions(stb PRIVATE LIBIEN_STB_ENABLE_IMAGE)
endif()

if(LIBIEN_BUILD_FONTS)
    target_compile_definitions(stb PRIVATE LIBIEN_STB_ENABLE_IMAGE LIBIEN_STB_ENABLE_FONTS)
endif()

set_target_properties(stb PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)