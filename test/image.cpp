#include "catch2/catch_message.hpp"
#include <ien/image/image.hpp>

#include <catch2/catch_test_macros.hpp>

const uint8_t rgba_data[] = {
    0, 1, 2, 3,
    0, 1, 2, 3,
    0, 1, 2, 3,
    0, 1, 2, 3
};

const uint8_t rgba_data_shuffled[] = {
    0, 3, 2, 1,
    0, 3, 2, 1,
    0, 3, 2, 1,
    0, 3, 2, 1
};

const ien::image_shuffle rgba_shuffle = {
    .indices = { 0, 3, 2, 1 }
};

const uint8_t rgb_data[] = {
    0, 1, 2,
    0, 1, 2,
    0, 1, 2,
    0, 1, 2
};

const uint8_t rgb_data_shuffled[] = {
    1, 2, 0,
    1, 2, 0,
    1, 2, 0,
    1, 2, 0
};

const ien::image_shuffle rgb_shuffle = {
    .indices = { 1, 2, 0 }
};

const uint8_t rg_data[] = {
    0, 1,
    0, 1,
    0, 1,
    0, 1
};

const uint8_t rg_data_shuffled[] = {
    1, 0,
    1, 0,
    1, 0,
    1, 0
};

const ien::image_shuffle rg_shuffle = {
    .indices = { 1, 0 }
};

TEST_CASE("image::shuffle")
{
    SECTION("rgba")
    {
        ien::image img(rgba_data, 2, 2, ien::image_format::RGBA);
        img.shuffle(rgba_shuffle);
        for(size_t i = 0; i < img.size(); ++i)
        {
            REQUIRE(img.data()[i] == rgba_data_shuffled[i]);
        }
    };

    SECTION("rgb")
    {
        ien::image img(rgb_data, 2, 2, ien::image_format::RGB);
        img.shuffle(rgb_shuffle);
        for(size_t i = 0; i < img.size(); ++i)
        {
            REQUIRE(img.data()[i] == rgb_data_shuffled[i]);
        }
    };

    SECTION("rg")
    {
        ien::image img(rg_data, 2, 2, ien::image_format::RG);
        img.shuffle(rg_shuffle);
        for(size_t i = 0; i < img.size(); ++i)
        {
            INFO(i);
            REQUIRE(img.data()[i] == rg_data_shuffled[i]);
        }
    };
};