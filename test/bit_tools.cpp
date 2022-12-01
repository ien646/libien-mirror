#include <ien/bit_tools.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("bitmask")
{
    SECTION("empty")
    {
        uint8_t x = ien::bitmask<uint8_t>();
        REQUIRE(x == static_cast<uint8_t>(0));
    };

    SECTION("single bit")
    {
        uint8_t x0 = ien::bitmask<uint8_t>(0);
        uint8_t x1 = ien::bitmask<uint8_t>(1);
        uint8_t x7 = ien::bitmask<uint8_t>(7);
        REQUIRE(x0 == static_cast<uint8_t>(0x01));
        REQUIRE(x1 == static_cast<uint8_t>(0x02));
        REQUIRE(x7 == static_cast<uint8_t>(0x80));
    };

    SECTION("multiple bits")
    {
        uint8_t x01 = ien::bitmask<uint8_t>(0, 1);
        uint8_t x37 = ien::bitmask<uint8_t>(3, 7);
        REQUIRE(x01 == static_cast<uint8_t>(0x02 + 0x01));
        REQUIRE(x37 == static_cast<uint8_t>(0x80 + 0x08));
    };

    SECTION("out of bounds")
    {
        uint8_t x08 = ien::bitmask<uint8_t>(8);
        REQUIRE(x08 == static_cast<uint8_t>(0));
    }
};