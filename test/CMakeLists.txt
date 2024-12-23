set(LIBIEN_TEST_SOURCES
    "algorithm.cpp"
    "arithmetic.cpp"
    "bit_tools.cpp"
    #"fs_utils.cpp"
    "initializers.cpp"
    "lang_utils.cpp"
    "net_utils.cpp"
    "serialization.cpp"
	"str_utils.cpp"
    "svo_vector.cpp"
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
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(LIBIEN_BENCH_SOURCES
    "benchmark/str_utils.cpp"
)

add_executable(libien_benchmarks ${LIBIEN_BENCH_SOURCES})
target_link_libraries(libien_benchmarks PRIVATE Catch2WithMain libien)
set_target_properties(libien_benchmarks PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
