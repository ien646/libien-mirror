#include "catch2/catch_approx.hpp"
#include "catch2/catch_message.hpp"
#include "ien/image/image_format.hpp"
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

const uint8_t ad_rgba_data0[] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

const uint8_t ad_rgba_data1[] = {
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255
};

const uint8_t ad_rgba_data2[] = {
    254, 254, 254, 254,
    254, 254, 254, 254,
    254, 254, 254, 254,
    254, 254, 254, 254
};

TEST_CASE("absolute_difference")
{
    SECTION("absolute_difference_0-1")
    {
        ien::image img0(ad_rgba_data0, 2, 2, ien::image_format::RGBA);
        ien::image img1(ad_rgba_data1, 2, 2, ien::image_format::RGBA);

        float diff = img0.absolute_difference(img1);
        REQUIRE(diff == Catch::Approx(255.0F / 255));
    };

    SECTION("absolute_difference_1-2")
    {
        ien::image img1(ad_rgba_data1, 2, 2, ien::image_format::RGBA);
        ien::image img2(ad_rgba_data2, 2, 2, ien::image_format::RGBA);

        float diff = img1.absolute_difference(img2);
        REQUIRE(diff == Catch::Approx(1.0F / 255).margin(0.000001F));
    };

    SECTION("absolute_difference_0-2")
    {
        ien::image img0(ad_rgba_data0, 2, 2, ien::image_format::RGBA);
        ien::image img2(ad_rgba_data2, 2, 2, ien::image_format::RGBA);

        float diff = img0.absolute_difference(img2);
        REQUIRE(diff == Catch::Approx(254.0F / 255).margin(0.000001F));
    };
};