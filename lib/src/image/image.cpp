#include <ien/image/image.hpp>

#include <ien/image/image.hpp>
#include <ien/image/image_format.hpp>

#include <ien/io_utils.hpp>
#include <ien/platform.hpp>
#include <ien/serialization.hpp>

#include <stb_image.h>
#include <stb_image_resize2.h>
#include <stb_image_write.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

#ifdef IEN_ARCH_X86_64
    #include <emmintrin.h>
#endif

namespace ien
{
    const std::string IEN_RAW_TAGGED_SIGNATURE = "IRIS";

#pragma pack(push, 1)
    struct raw_tagged_image_header
    {
        std::array<char, 4> signature;
        uint32_t width;
        uint32_t height;
        uint8_t channels;
    };
#pragma pack(pop)
    static_assert(sizeof(raw_tagged_image_header) == 13);

    image::image(size_t width, size_t height, image_format fmt)
        : _width(width)
        , _height(height)
        , _format(fmt)
    {
        _data = reinterpret_cast<uint8_t*>(malloc(_width * _height * image_format_channels(_format)));
        assert(width > 0 && height > 0);
    }

    image::image(const std::string& path, image_format fmt, image_load_mode load_mode)
    {
        assert(!path.empty());
        _format = ien::image_format::RGBA;
        if (load_mode == image_load_mode::IMAGE)
        {
            int image_channels;
            int w, h;
            _data = stbi_load(path.c_str(), &w, &h, &image_channels, 4);
            if (_data == nullptr)
            {
                throw std::logic_error(std::format("Unable to open image at '{}' ({})", path, stbi_failure_reason()));
            }

            _width = (size_t)w;
            _height = (size_t)h;

            if (fmt != image_format::RGBA)
            {
                *this = cast_format(fmt);
            }
        }
        else // if (load_mode == image_load_mode::RAW)
        {
            const std::optional<std::vector<std::byte>> data = ien::read_file_binary(path);
            if (!data)
            {
                throw std::logic_error(std::format("Unable to open image file at '{}'", path));
            }

            if (data->size() < sizeof(raw_tagged_image_header))
            {
                throw std::logic_error("Invalid tagged raw image file");
            }

            std::string signature;
            uint32_t width = 0;
            uint32_t height = 0;
            uint8_t format = 0;

            ien::deserializer deserializer(std::span{ *data });
            deserializer.deserialize_into_buffer(signature.data(), IEN_RAW_TAGGED_SIGNATURE.size());
            if (signature != IEN_RAW_TAGGED_SIGNATURE)
            {
                throw std::logic_error("Raw tagged image file signature mismatch");
            }

            width = deserializer.deserialize<uint32_t>();
            height = deserializer.deserialize<uint32_t>();
            format = deserializer.deserialize<uint8_t>();

            if (width == 0 || height == 0)
            {
                throw std::logic_error("Invalid image dimensions");
            }

            _width = (size_t)width;
            _height = (size_t)height;
            _format = static_cast<image_format>(format);

            auto bytes = deserializer.deserialize<SERIALIZE_CONTAINER_SIZE_T>();
            _data = reinterpret_cast<uint8_t*>(malloc(bytes));
            deserializer.deserialize_into_buffer(_data, bytes);
        }
    }

    image::~image()
    {
        if (_data != nullptr)
        {
            free(_data);
            _data = nullptr;
        }
        _width = 0;
        _height = 0;
    }

    image::image(const void* data, size_t width, size_t height, image_format format)
        : _width(width)
        , _height(height)
        , _format(format)
    {
        assert(data != nullptr);
        assert(width > 0 && height > 0);

        _data = reinterpret_cast<uint8_t*>(malloc(_width * _height * image_format_channels(_format)));

        std::memcpy(_data, data, width * height * channel_count());
    }

    image::image(image&& mv_src) noexcept
    {
        *this = std::move(mv_src);
    }

    image& image::operator=(image&& mvsrc) noexcept
    {
        if (_data)
        {
            free(_data);
        }

        _data = mvsrc._data;
        _width = mvsrc._width;
        _height = mvsrc._height;
        _format = mvsrc._format;

        mvsrc._data = nullptr;
        mvsrc._width = 0;
        mvsrc._height = 0;

        return *this;
    }

    image image::resize(size_t width, size_t height, resize_filter filter) const
    {
        assert(width > 0 && height > 0);

        image result(width, height, _format);
        const bool ok = stbir_resize(
                            _data,
                            (int)_width,
                            (int)_height,
                            0,
                            result._data,
                            static_cast<int>(width),
                            static_cast<int>(height),
                            0,
                            static_cast<stbir_pixel_layout>(channel_count()),
                            stbir_datatype::STBIR_TYPE_UINT8,
                            stbir_edge::STBIR_EDGE_WRAP,
                            static_cast<stbir_filter>(filter)
                        ) != nullptr;

        if (!ok)
        {
            throw std::logic_error("Failure resizing image!");
        }
        return result;
    }

    void image::write_to_file_png(const std::string& path, int compression) const
    {
        assert(!path.empty());
        assert(compression > 0);
        const int aux = stbi_write_png_compression_level;
        stbi_write_png_compression_level = compression;
        stbi_write_png(path.c_str(), (int)_width, (int)_height, channel_count(), _data, (int)(_width * channel_count()));
        stbi_write_png_compression_level = aux;
    }

    void image::write_to_file_bmp(const std::string& path) const
    {
        assert(!path.empty());
        stbi_write_bmp(path.c_str(), (int)_width, (int)_height, channel_count(), _data);
    }

    void image::write_to_file_tga(const std::string& path) const
    {
        assert(!path.empty());
        stbi_write_tga(path.c_str(), (int)_width, (int)_height, channel_count(), _data);
    }

    void image::write_to_file_jpg(const std::string& path, int quality) const
    {
        assert(!path.empty());
        stbi_write_jpg(path.c_str(), (int)_width, (int)_height, channel_count(), _data, quality);
    }

    void image::write_to_file_raw_tagged(const std::string& path) const
    {
        assert(!path.empty());
        const auto width = static_cast<uint32_t>(_width);
        const auto height = static_cast<uint32_t>(_height);
        const auto fmt = static_cast<uint8_t>(format());

        ien::serializer serializer;
        serializer.serialize_buffer<char>(IEN_RAW_TAGGED_SIGNATURE);
        serializer.serialize(width);
        serializer.serialize(height);
        serializer.serialize(fmt);
        serializer.serialize(std::span(_data, size()));

        ien::write_file_binary(path, serializer.data());
    }

    ien::image image::copy_rect(size_t x, size_t y, size_t w, size_t h) const
    {
        assert(w + x <= width());
        assert(h + y <= height());

        ien::image result(w, h, _format);

        for (size_t py = 0; py < h; ++py)
        {
            uint8_t* row_dst_ptr = result.data(0, py);
            const uint8_t* row_src_ptr = data(x, y + py);
            std::memcpy(row_dst_ptr, row_src_ptr, image_format_channels(_format) * w);
        }
        return result;
    }

    image image::extract_channel(size_t channel_index) const
    {
        assert(channel_index <= 4);
        if (channel_index >= channel_count())
        {
            throw std::out_of_range("Invalid image channel index");
        }

        image result(_width, _height, image_format::R);
        const size_t channels = channel_count();
        const size_t pixels = pixel_count();
        for (size_t i = 0, j = channel_index; i < pixels; ++i, j += channels)
        {
            result._data[i] = _data[j];
        }
        return result;
    }

    image image::cast_format(image_format target_format) const
    {
        if (_format == target_format)
        {
            image result(_width, _height, target_format);
            std::memcpy(result.data(), this->data(), _width * _height * image_format_channels(target_format));
            return result;
        }

        const auto source_channels = image_format_channels(_format);
        const auto target_channels = image_format_channels(target_format);
        image result(_width, _height, target_format);
        for (size_t srcpxi = 0; srcpxi < _width * _height; ++srcpxi)
        {
            for (size_t tgt_ch_index = 0; tgt_ch_index < target_channels; ++tgt_ch_index)
            {
                if (tgt_ch_index <= source_channels)
                {
                    result.data(srcpxi)[tgt_ch_index] = data(srcpxi)[tgt_ch_index];
                }
                else
                {
                    result.data(srcpxi)[tgt_ch_index] = 0;
                }
            }
        }

        return result;
    }

    void image::shuffle(const image_shuffle& op)
    {
        const size_t pixels = pixel_count();
        const size_t channels = channel_count();
        if (channels == 1)
        {
            return;
        }

        const auto shuffle_index_std = [&](size_t pixel_index) {
            uint8_t* pxptr = _data + (pixel_index * channels);
            std::array<uint8_t, 4> temp;
            for (size_t i = 0; i < channels; ++i)
            {
                temp[i] = pxptr[op.indices[i]];
            }

            for (size_t i = 0; i < channels; ++i)
            {
                pxptr[i] = temp[i];
            }
        };

        for (size_t i = 0; i < pixels; ++i)
        {
            shuffle_index_std(i);
        }
    }

    float image::absolute_difference(const image& other) const
    {
        if (this->width() != other.width() || this->height() != other.height())
        {
            throw std::logic_error("Image dimensions do not match");
        }

        if (this->format() != other.format())
        {
            throw std::logic_error("Image formats do not match");
        }

        if (this->format() != image_format::RGBA)
        {
            throw std::logic_error("absolute_difference(...) is only supported for RGBA formats, use cast_format(...)");
        }

        float accum = 0;

#ifdef IEN_ARCH_X86_64 // guaranteed sse2 on x64, measured about 2~3 times faster on 1024x1024 images
        const uint8_t* px_a0 = data();
        const uint8_t* px_b0 = other.data();

        assert(ien::is_ptr_aligned(px_a0, 16));
        assert(ien::is_ptr_aligned(px_b0, 16));

        const __m128 vmul_div255 = _mm_set1_ps(1.0F / 255);
        const __m128 vand_fpsign = _mm_set1_ps(std::bit_cast<float>(~0x80000000));
        const __m128 vmul_div3 = _mm_set1_ps(1.0F / 3);

        for (size_t i = 0; i < (pixel_count() - (pixel_count() % 4)); i += 4)
        {
            const uint8_t* px_a = px_a0 + (i * 4);
            const uint8_t* px_b = px_b0 + (i * 4);

            const __m128i
                va_r_u32 = _mm_set_epi8(0, 0, 0, px_a[0], 0, 0, 0, px_a[4], 0, 0, 0, px_a[8], 0, 0, 0, px_a[12]);
            const __m128i
                va_g_u32 = _mm_set_epi8(0, 0, 0, px_a[1], 0, 0, 0, px_a[5], 0, 0, 0, px_a[9], 0, 0, 0, px_a[13]);
            const __m128i
                va_b_u32 = _mm_set_epi8(0, 0, 0, px_a[2], 0, 0, 0, px_a[6], 0, 0, 0, px_a[10], 0, 0, 0, px_a[14]);

            const __m128i
                vb_r_u32 = _mm_set_epi8(0, 0, 0, px_b[0], 0, 0, 0, px_b[4], 0, 0, 0, px_b[8], 0, 0, 0, px_b[12]);
            const __m128i
                vb_g_u32 = _mm_set_epi8(0, 0, 0, px_b[1], 0, 0, 0, px_b[5], 0, 0, 0, px_b[9], 0, 0, 0, px_b[13]);
            const __m128i
                vb_b_u32 = _mm_set_epi8(0, 0, 0, px_b[2], 0, 0, 0, px_b[6], 0, 0, 0, px_b[10], 0, 0, 0, px_b[14]);

            __m128 va_r_f32 = _mm_cvtepi32_ps(va_r_u32);
            __m128 va_g_f32 = _mm_cvtepi32_ps(va_g_u32);
            __m128 va_b_f32 = _mm_cvtepi32_ps(va_b_u32);

            __m128 vb_r_f32 = _mm_cvtepi32_ps(vb_r_u32);
            __m128 vb_g_f32 = _mm_cvtepi32_ps(vb_g_u32);
            __m128 vb_b_f32 = _mm_cvtepi32_ps(vb_b_u32);

            va_r_f32 = _mm_mul_ps(va_r_f32, vmul_div255);
            va_g_f32 = _mm_mul_ps(va_g_f32, vmul_div255);
            va_b_f32 = _mm_mul_ps(va_b_f32, vmul_div255);

            vb_r_f32 = _mm_mul_ps(vb_r_f32, vmul_div255);
            vb_g_f32 = _mm_mul_ps(vb_g_f32, vmul_div255);
            vb_b_f32 = _mm_mul_ps(vb_b_f32, vmul_div255);

            __m128 vdiff_r_f32 = _mm_sub_ps(va_r_f32, vb_r_f32);
            __m128 vdiff_g_f32 = _mm_sub_ps(va_g_f32, vb_g_f32);
            __m128 vdiff_b_f32 = _mm_sub_ps(va_b_f32, vb_b_f32);

            vdiff_r_f32 = _mm_and_ps(vdiff_r_f32, vand_fpsign);
            vdiff_g_f32 = _mm_and_ps(vdiff_g_f32, vand_fpsign);
            vdiff_b_f32 = _mm_and_ps(vdiff_b_f32, vand_fpsign);

            const __m128 vdiff_rgb_f32 = _mm_add_ps(_mm_add_ps(vdiff_r_f32, vdiff_g_f32), vdiff_b_f32);
            const __m128 vdiff_avg_f32 = _mm_mul_ps(vdiff_rgb_f32, vmul_div3);

            const __m128 vdiff_avg_f32_01 = _mm_add_ps(
                vdiff_avg_f32,
                _mm_shuffle_ps(vdiff_avg_f32, vdiff_avg_f32, _MM_SHUFFLE(1, 1, 1, 1))
            );

            const __m128 vdiff_avg_f32_23 = _mm_add_ps(
                _mm_shuffle_ps(vdiff_avg_f32, vdiff_avg_f32, _MM_SHUFFLE(2, 2, 2, 2)),
                _mm_shuffle_ps(vdiff_avg_f32, vdiff_avg_f32, _MM_SHUFFLE(3, 3, 3, 3))
            );

            const __m128 vdiff_avg_f32_0123 = _mm_add_ps(vdiff_avg_f32_01, vdiff_avg_f32_23);

            accum += _mm_cvtss_f32(vdiff_avg_f32_0123);
        }

        // Non vectorizable remainder
        for (size_t i = (pixel_count() - (pixel_count() % 4)); i < pixel_count(); ++i)
        {
            const uint8_t* px_a = data() + (i * 4);
            const uint8_t* px_b = other.data() + (i * 4);
            const float ra = (float)px_a[0] / 255;
            const float ga = (float)px_a[1] / 255;
            const float ba = (float)px_a[2] / 255;

            const float rb = (float)px_b[0] / 255;
            const float gb = (float)px_b[1] / 255;
            const float bb = (float)px_b[2] / 255;

            const float dr = std::abs(ra - rb);
            const float dg = std::abs(ga - gb);
            const float db = std::abs(ba - bb);

            const float davg = (dr + dg + db) / 3;
            accum += davg;
        }
#else
        for (size_t i = 0; i < img_a.pixel_count(); ++i)
        {
            const uint8_t* px_a = img_a.data() + (i * 4);
            const uint8_t* px_b = img_b.data() + (i * 4);
            float ra = (float)px_a[0] / 255;
            float ga = (float)px_a[1] / 255;
            float ba = (float)px_a[2] / 255;

            float rb = (float)px_b[0] / 255;
            float gb = (float)px_b[1] / 255;
            float bb = (float)px_b[2] / 255;

            float dr = std::abs(ra - rb);
            float dg = std::abs(ga - gb);
            float db = std::abs(ba - bb);

            float davg = (dr + dg + db) / 3;
            accum += davg;
        }
#endif
        return accum / static_cast<float>(pixel_count());
    }

    void image::flip_axis_y()
    {
        const size_t row_size = _width * image_format_channels(_format);
        for (size_t i = 0; i < _height / 2; ++i)
        {
            uint8_t* row_up = _data + (row_size * i);
            uint8_t* row_down = _data + (row_size * (_height - i - 1));
            std::swap_ranges(row_up, row_up + row_size, row_down);
        }
    }

    image image::pad(uint32_t x, uint32_t y, uint32_t rgba) const
    {
        image result(_width + x, _height + x, _format);
        const auto source_row_size = _width * image_format_channels(_format);

        for (size_t row = 0; row < _height; ++row)
        {
            auto* target_ptr = result.data(0, row);
            std::memcpy(target_ptr, data(0, row), source_row_size);
            for (size_t i = 0; i < image_format_channels(_format); ++i)
            {
                target_ptr[source_row_size + i] = static_cast<uint8_t>(rgba >> (8 * (3 - i)));
            }
        }

        std::vector<uint8_t> padded_row_data;
        padded_row_data.resize((_width + x) * image_format_channels(_format));

        for (size_t i = 0; i < padded_row_data.size(); ++i)
        {
            padded_row_data[i] = static_cast<uint8_t>(rgba >> (8 * (3 - i)));
        }

        for (size_t i = 0; i < y; ++i)
        {
            std::memcpy(result.data(0, _height + i), padded_row_data.data(), padded_row_data.size());
        }

        return result;
    }

    bool is_valid_image_file(const std::string& path)
    {
        return stbi_info(path.c_str(), nullptr, nullptr, nullptr) == 1 ? true : false;
    }

    std::optional<image_info> get_image_info(const std::string& path)
    {
        if (image_info result; stbi_info(path.c_str(), &result.width, &result.height, &result.channels))
        {
            return result;
        }
        return std::nullopt;
    }
} // namespace ien