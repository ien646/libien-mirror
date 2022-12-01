add_executable(libien_tests
    "bit_tools.cpp"
    "lang_utils.cpp"
    "image.cpp"
    "net_utils.cpp"
	"str_utils.cpp"
    "lang_utils.cpp"
)

if(LIBIEN_BUILD_IMAGE)
    target_sources(libien_tests PRIVATE "image.cpp")
endif()

set_target_properties(libien_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(libien_tests PRIVATE Catch2WithMain libien)