#include <catch2/catch_test_macros.hpp>

#include <ctime>
#include <ien/fs_utils.hpp>
#include <ien/platform.hpp>

#include <chrono>
#include <cstdio>
#include <string>

#ifdef IEN_OS_WIN
    #define timegm _mkgmtime
#endif

TEST_CASE("fs_utils")
{
    SECTION("get_current_user_homedir()")
    {
        REQUIRE(!ien::get_current_user_homedir().empty());
    };

    GIVEN("A file of 8 bytes, with mtime 2022-12-11T01:03:04 (UTC) and atime 2022-12-11T01:02:03 (UTC)")
    {
        std::string file = "test_data/file";
        WHEN("Asking for the file's mtime")
        {
            auto mtime = ien::get_file_mtime(file);
            THEN("The resulting mtime is correct")
            {
                std::tm tm;
                std::memset(&tm, 0, sizeof(std::tm));
                tm.tm_year = 2022 - 1900;
                tm.tm_mon = 12 - 1;
                tm.tm_mday = 11;
                tm.tm_hour = 1;
                tm.tm_min = 3;
                tm.tm_sec = 4;
                std::time_t time = timegm(&tm);
                REQUIRE(time == mtime);
            };
        };

        WHEN("Asking for the file's atime")
        {
            auto atime = ien::get_file_atime(file);
            THEN("The resulting atime is correct")
            {
                std::tm tm;
                std::memset(&tm, 0, sizeof(std::tm));
                tm.tm_year = 2022 - 1900;
                tm.tm_mon = 12 - 1;
                tm.tm_mday = 11;
                tm.tm_hour = 1;
                tm.tm_min = 2;
                tm.tm_sec = 3;
                std::time_t time = std::mktime(&tm);
                REQUIRE(time == atime);
            };
        };

        WHEN("Asking for the file's size")
        {
            auto size = ien::get_file_size(file);
            THEN("The resulting size is correct")
            {                
                REQUIRE(size == 8);
            };
        };
    };
};