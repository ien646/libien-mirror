#pragma once

#include <stdexcept>
namespace ien
{
    enum class image_format
    {
        R,
        RG,
        RGB,
        RGBA,
    };

    constexpr image_format channels_to_image_format(unsigned char channel_count)
    {
        switch (channel_count)
        {
        case 1:
            return image_format::R;
        case 2:
            return image_format::RG;
        case 3:
            return image_format::RGB;
        case 4:
            return image_format::RGBA;
        default:
            throw std::invalid_argument("Invalid channel_count");
        }
    }

    constexpr unsigned char image_format_channels(image_format format)
    {
        switch (format)
        {
        case image_format::R:
            return 1;
        case image_format::RG:
            return 2;
        case image_format::RGB:
            return 3;
        case image_format::RGBA:
            return 4;
        default:
            throw std::invalid_argument("Invalid image_format");
        }
    }
} // namespace ien