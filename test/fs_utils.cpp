#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <ien/fs_utils.hpp>

TEST_CASE("fs_utils")
{
    SECTION("get_current_user_homedir()")
    {
        REQUIRE(!ien::get_current_user_homedir().empty());
    };
};