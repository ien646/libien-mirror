add_library(stb src/stb.cpp)

target_include_directories(stb PUBLIC include)

set_target_properties(stb PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)