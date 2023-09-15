set(LIBIEN_TEST_SOURCES
    "algorithm.cpp"
    "arithmetic.cpp"
    "bit_tools.cpp"
    "fs_utils.cpp"
    "lang_utils.cpp"
    "net_utils.cpp"
    "serialization.cpp"
	"str_utils.cpp"    
)

if(LIBIEN_BUILD_IMAGE)
    set(LIBIEN_TEST_SOURCES ${LIBIEN_TEST_SOURCES} "image.cpp")
endif()

if(LIBIEN_BUILD_SFTP)
    # ...
endif()

add_executable(libien_tests ${LIBIEN_TEST_SOURCES})
target_link_libraries(libien_tests PRIVATE Catch2WithMain libien)
set_target_properties(libien_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})