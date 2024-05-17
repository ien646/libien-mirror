#pragma once

#include <ien/image/image_format.hpp>

#include <array>
#include <cstdint>

namespace ien
{
    struct image_shuffle
	{
		std::array<uint8_t, 4> indices = {0, 1, 2, 3};
	};

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
        image_data(image_data&& mvsrc) noexcept;
        virtual ~image_data();
        image_data& operator=(image_data&& mvsrc) noexcept;

        inline size_t width() const { return _width; }
		inline size_t height() const { return _height; }

        inline image_format format() const { return _format; }

		inline size_t pixel_count() const { return _width * _height; }
		inline unsigned char channel_count() const { return image_format_channels(_format); }
		inline size_t size() const { return pixel_count() * channel_count(); }

		inline unsigned char* data() { return _data; }
		inline const unsigned char* data() const { return _data; }

        inline unsigned char* data(size_t pxindex) { return _data + (pxindex * channel_count()); }
        inline const unsigned char* data(size_t pxindex) const { return _data + (pxindex * channel_count()); }

        inline size_t pixel_index(size_t x, size_t y) const { return (y * _width) + x; }

        inline unsigned char* data(size_t x, size_t y) { return _data + (pixel_index(x, y) * channel_count()); }
        inline const unsigned char* data(size_t x, size_t y) const { return _data + (pixel_index(x, y) * channel_count()); }

        image_data extract_channel(size_t channel_index) const;

        void cast_format(image_format target_format);

        void shuffle(const image_shuffle&);

        float absolute_difference(const image_data& other) const;

        void flip_axis_y();        
    };
}