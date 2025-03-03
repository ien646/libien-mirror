#pragma once

#include <ien/image/image_format.hpp>
#include <ien/image/resize_filter.hpp>

#include <array>
#include <cstdint>
#include <optional>
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

        size_t width() const { return _width; }
        size_t height() const { return _height; }

        image_format format() const { return _format; }

        size_t pixel_count() const { return _width * _height; }
        unsigned char channel_count() const { return image_format_channels(_format); }
        size_t size() const { return pixel_count() * channel_count(); }

        unsigned char* data() { return _data; }
        const unsigned char* data() const { return _data; }

        unsigned char* data(size_t pxindex) { return _data + (pxindex * channel_count()); }
        const unsigned char* data(size_t pxindex) const { return _data + (pxindex * channel_count()); }

        size_t pixel_index(size_t x, size_t y) const { return (y * _width) + x; }

        unsigned char* data(size_t x, size_t y) { return _data + (pixel_index(x, y) * channel_count()); }
        const unsigned char* data(size_t x, size_t y) const
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

    bool is_valid_image_file(const std::string& path);

    struct image_info
    {
        int width = 0;
        int height = 0;
        int channels = 0;
    };
    std::optional<image_info> get_image_info(const std::string& path);
} // namespace ien