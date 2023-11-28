add_executable(libien-playground "playground.cpp")
target_link_libraries(libien-playground PRIVATE libien)
set_target_properties(libien-playground PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)