#include <ien/arithmetic.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>

TEST_CASE("safe_*")
{
    SECTION("safe_add")
    {
        uint8_t val_a = 255;
        uint8_t val_b = 1;
        int vsum = ien::safe_add<int>(val_a, val_b);
        REQUIRE(vsum == 256);
    };

    SECTION("safe_mul")
    {
        uint8_t val_a = 128;
        int val_b = 2;
        int vmul = ien::safe_mul<int>(val_a, val_b);
    }
};

TEST_CASE("construct*")
{
    SECTION("construct2 8+8=16")
    {
        uint8_t a = 0xAA;
        uint8_t b = 0x77;
        uint16_t ab = ien::construct2<uint16_t>(a, b);
        REQUIRE(ab == 0xAA77);
    };

    SECTION("construct2 16+16=32")
    {
        uint16_t a = 0xAA10;
        uint16_t b = 0x77CB;
        uint32_t ab = ien::construct2<uint32_t>(a, b);
        REQUIRE(ab == 0xAA1077CB);
    };

    SECTION("construct2 32+32=64")
    {
        uint32_t a = 0xAA102398;
        uint32_t b = 0x77CB1634;
        uint64_t ab = ien::construct2<uint64_t>(a, b);
        REQUIRE(ab == 0xAA10239877CB1634);
    };

    SECTION("construct4 8+8+8+8=32")
    {
        uint8_t a = 0xAA;
        uint8_t b = 0x77;
        uint8_t c = 0x12;
        uint8_t d = 0x64;
        uint32_t abcd = ien::construct4<uint32_t>(a, b, c, d);
        REQUIRE(abcd == 0xAA771264);
    };

    SECTION("construct4 16+16+16+16=64")
    {
        uint16_t a = 0xAA12;
        uint16_t b = 0x7734;
        uint16_t c = 0x1245;
        uint16_t d = 0x6456;
        uint64_t abcd = ien::construct4<uint64_t>(a, b, c, d);
        REQUIRE(abcd == 0xAA12773412456456);
    };
};