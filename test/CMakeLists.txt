add_executable(libien_tests
	"str_utils.cpp"
    "lang_utils.cpp"
)

set_target_properties(libien_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(libien_tests PRIVATE Catch2WithMain libien)