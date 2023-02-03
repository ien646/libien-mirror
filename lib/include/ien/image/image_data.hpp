#pragma once

#include <ien/image/image_format.hpp>

#include <cstdint>

namespace ien
{
    class image_data
    {
    protected:
        uint8_t* _data = nullptr;
		size_t _width = 0;
		size_t _height = 0;
		image_format _format = image_format::RGBA;

    public:
        constexpr image_data() {};
        image_data(size_t w, size_t h, image_format fmt);
        image_data(const image_data&) = delete;
        image_data(image_data&& mvsrc);
        virtual ~image_data();
        image_data& operator=(image_data&& mvsrc);
    };
}