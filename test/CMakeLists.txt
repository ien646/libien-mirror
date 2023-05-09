set(LIBIEN_TEST_SOURCES
    "alloc.cpp"
    "bit_tools.cpp"
    "lang_utils.cpp"
    "net_utils.cpp"
	"str_utils.cpp"
    "lang_utils.cpp"
)

if(LIBIEN_BUILD_IMAGE)
    set(LIBIEN_TEST_SOURCES ${LIBIEN_TEST_SOURCES} "image.cpp")
endif()

if(LIBIEN_BUILD_SFTP)
    # ...
endif()

add_executable(libien_tests ${LIBIEN_TEST_SOURCES})

set_target_properties(libien_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(libien_tests PRIVATE Catch2WithMain libien)