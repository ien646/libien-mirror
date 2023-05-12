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

macro(DEFINE_TEST TAG)
    add_executable("libien_tests_${TAG}" "${TAG}.cpp")
    target_link_libraries("libien_tests_${TAG}" PRIVATE Catch2WithMain libien)
    set_target_properties("libien_tests_${TAG}" PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    add_test(NAME "libien_tests_${TAG}" COMMAND "libien_tests_${TAG}")
endmacro()

DEFINE_TEST(alloc)
DEFINE_TEST(bit_tools)
DEFINE_TEST(lang_utils)
DEFINE_TEST(net_utils)
DEFINE_TEST(str_utils)

if(LIBIEN_BUILD_IMAGE)
    DEFINE_TEST(image)
endif()