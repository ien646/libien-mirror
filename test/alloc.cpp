#include <ien/alloc.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("stack alloc")
{
    SECTION("zero size doesn't fail")
    {
        void* ptr = ZYNR_STACK_ALLOC(0);
        REQUIRE(ptr != nullptr);
        ZYNR_STACK_FREE(ptr);
    };

    SECTION("non zero returns valid ptr")
    {
        void* ptr = ZYNR_STACK_ALLOC(64);
        REQUIRE(ptr != nullptr);
        ZYNR_STACK_FREE(ptr);
    }
}