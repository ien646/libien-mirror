#include <bit>
#include <cstdint>
#include <ien/serialization.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Serialization")
{
    SECTION("Serialization sequence")
    {
        uint32_t u32 = 0xA0B1C2D3;
        uint16_t u16 = 0x0A1B;
        uint8_t u8 = 0xD7;
        std::string str = "IRIS";

        float f32 = 0.75F;
        uint32_t f32_bitcast = std::bit_cast<uint32_t>(f32);

        std::vector<uint16_t> vec16 = { 0xABCD, 0xBADD, 0xD066, 0x7877 };

        std::vector<uint8_t> expected = {
            /*u32*/         0xA0, 0xB1, 0xC2, 0xD3,
            /*u16*/         0x0A, 0x1B,
            /*u8*/          0xD7,
            /*char[4]*/     'I', 'R', 'I', 'S',
            /*f32*/         static_cast<uint8_t>(f32_bitcast >> 24), static_cast<uint8_t>(f32_bitcast >> 16), static_cast<uint8_t>(f32_bitcast >> 8), static_cast<uint8_t>(f32_bitcast),
            /*vec16*/       0xAB, 0xCD, 0xBA, 0xDD, 0xD0, 0x66, 0x78, 0x77
        };

        ien::serializer serializer;
        serializer.serialize(u32);
        serializer.serialize(u16);
        serializer.serialize(u8);
        serializer.serialize(str);
        serializer.serialize(f32);
        serializer.serialize(vec16);

        REQUIRE(serializer.data() == expected);
    };
};