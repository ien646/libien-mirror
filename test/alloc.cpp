#include <ien/alloc.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("stack alloc")
{
    SECTION("zero size doesn't fail")
    {
        void* ptr = IEN_STACK_ALLOC(0);
        REQUIRE(ptr != nullptr);
        IEN_STACK_FREE(ptr);
    };

    SECTION("non zero returns valid ptr")
    {
        void* ptr = IEN_STACK_ALLOC(64);
        REQUIRE(ptr != nullptr);
        IEN_STACK_FREE(ptr);
    }
}