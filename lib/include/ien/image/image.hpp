#pragma once

#include <ien/image/image_format.hpp>

#include <array>
#include <cstdint>
#include <string>

#ifndef LIBIEN_BUILD_IMAGE
    #error "libien must be compiled with CMake option 'LIBIEN_BUILD_IMAGE' to use image functionality"
#endif

namespace ien
{
    enum class image_load_mode
    {
        IMAGE,
        RAW
    };

    enum class resize_filter
    {
        DEFAULT = 0,      // use same filter type that easy-to-use API chooses
        BOX = 1,          // A trapezoid w/1-pixel wide ramps, same result as box for integer scale ratios
        TRIANGLE = 2,     // On upsampling, produces same results as bilinear texture filtering
        CUBICBSPLINE = 3, // The cubic b-spline (aka Mitchell-Netrevalli with B=1,C=0), gaussian-esque
        CATMULLROM = 4,   // An interpolating cubic spline
        MITCHELL = 5,     // Mitchell-Netrevalli filter with B=1/3, C=1/3
        POINT_SAMPLE = 6  // Simple point sampling
    };

    struct image_shuffle
    {
        std::array<uint8_t, 4> indices = { 0, 1, 2, 3 };
    };

    class image
    {
    public:
        image(size_t width, size_t height, image_format = image_format::RGBA);
        explicit image(
            const std::string& path,
            image_format fmt = image_format::RGBA,
            image_load_mode load_mode = image_load_mode::IMAGE
        );
        image(const void* data, size_t width, size_t height, image_format format);
        image(const image& cp_src) = delete;
        image(image&& mv_src) noexcept;
        image& operator=(image&& mvsrc) noexcept;
        virtual ~image();

        [[nodiscard]] image resize(size_t width, size_t height, resize_filter = resize_filter::DEFAULT) const;

        void write_to_file_png(const std::string& path, int compression = 8) const;
        void write_to_file_bmp(const std::string& path) const;
        void write_to_file_tga(const std::string& path) const;
        void write_to_file_jpg(const std::string& path, int quality = 100) const;
        void write_to_file_raw_tagged(const std::string& path) const;

        float absolute_difference(const image& other) const;

        [[nodiscard]] ien::image copy_rect(size_t x, size_t y, size_t w, size_t h) const;

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
        inline const unsigned char* data(size_t x, size_t y) const
        {
            return _data + (pixel_index(x, y) * channel_count());
        }

        image extract_channel(size_t channel_index) const;

        image cast_format(image_format target_format) const;

        void shuffle(const image_shuffle&);

        void flip_axis_y();

        image pad(uint32_t x, uint32_t y, uint32_t rgba) const;

    protected:
        uint8_t* _data = nullptr;
        size_t _width = 0;
        size_t _height = 0;
        image_format _format = image_format::RGBA;
    };
} // namespace ien