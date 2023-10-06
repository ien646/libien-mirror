#include <ien/serialization.hpp>

#include <catch2/catch_test_macros.hpp>

#include <bit>
#include <cstdint>
#include <string>
#include <vector>

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
            /*str-size*/    0x00, 0x00, 0x00, 0x04, 
            /*srt-data*/    'I', 'R', 'I', 'S',
            /*f32*/         static_cast<uint8_t>(f32_bitcast >> 24), static_cast<uint8_t>(f32_bitcast >> 16), static_cast<uint8_t>(f32_bitcast >> 8), static_cast<uint8_t>(f32_bitcast),
            /*vec16-size*/  0x00, 0x00, 0x00, 0x04,
            /*vec16-data*/  0xAB, 0xCD, 0xBA, 0xDD, 0xD0, 0x66, 0x78, 0x77
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

    SECTION("Deserialization sequence")
    {
        uint32_t expected_u32 = 0xA0B1C2D3;
        uint16_t expected_u16 = 0x0A1B;
        uint8_t expected_u8 = 0xD7;
        std::string expected_str = "IRIS";

        float expected_f32 = 0.75F;
        uint32_t f32_bitcast = std::bit_cast<uint32_t>(expected_f32);

        std::vector<uint16_t> expected_vec16 = { 0xABCD, 0xBADD, 0xD066, 0x7877 };

        std::vector<uint8_t> data = {
            /*u32*/         0xA0, 0xB1, 0xC2, 0xD3,
            /*u16*/         0x0A, 0x1B,
            /*u8*/          0xD7,
            /*str-size*/    0x00, 0x00, 0x00, 0x04, 
            /*srt-data*/    'I', 'R', 'I', 'S',
            /*f32*/         static_cast<uint8_t>(f32_bitcast >> 24), static_cast<uint8_t>(f32_bitcast >> 16), static_cast<uint8_t>(f32_bitcast >> 8), static_cast<uint8_t>(f32_bitcast),
            /*vec16-size*/  0x00, 0x00, 0x00, 0x04,
            /*vec16-data*/  0xAB, 0xCD, 0xBA, 0xDD, 0xD0, 0x66, 0x78, 0x77
        };

        ien::deserializer deserializer{std::span<uint8_t>(data)};
        uint32_t u32 = deserializer.deserialize<uint32_t>();
        uint16_t u16 = deserializer.deserialize<uint16_t>();
        uint8_t u8 = deserializer.deserialize<uint8_t>();
        std::string str = deserializer.deserialize<std::string>();
        float f32 = deserializer.deserialize<float>();
        std::vector<uint16_t> vec16 = deserializer.deserialize<std::vector<uint16_t>>();

        REQUIRE(u32 == expected_u32);
        REQUIRE(u16 == expected_u16);
        REQUIRE(u8 == expected_u8);
        REQUIRE(f32 == expected_f32);
        REQUIRE(str == expected_str);
        REQUIRE(vec16 == expected_vec16);
    };
};