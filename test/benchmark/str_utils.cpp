#include <ien/str_utils.hpp>

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_all.hpp>

const std::string
    text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut "
           "labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco "
           "laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in "
           "voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat "
           "non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

TEST_CASE("ien::str_indices_of")
{
    BENCHMARK("space by char")
    {
        return ien::str_indices_of(text, ' ');
    };

    BENCHMARK("space by string")
    {
        return ien::str_indices_of(text, " ");
    };

    BENCHMARK("space by char - expect 70 hits")
    {
        return ien::str_indices_of<70>(text, ' ');
    };

    BENCHMARK("space by string - expect 70 hits")
    {
        return ien::str_indices_of<70>(text, " ");
    };
}

TEST_CASE("ien::str_split")
{
    BENCHMARK("space by char")
    {
        return ien::str_split(text, ' ');
    };

    BENCHMARK("space by string")
    {
        return ien::str_split(text, " ");
    };

        BENCHMARK("space by char - expect 70 hits")
    {
        return ien::str_split<70>(text, ' ');
    };

    BENCHMARK("space by string - expect 70 hits")
    {
        return ien::str_split<70>(text, " ");
    };
}