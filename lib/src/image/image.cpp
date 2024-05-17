#include <algorithm>
#include <cstdint>
#include <ien/image/image.hpp>
#include <ien/image/image_data.hpp>
#include <ien/image/image_format.hpp>

#include <ien/io_utils.hpp>
#include <ien/platform.hpp>
#include <ien/serialization.hpp>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <array>
#include <cassert>
#include <cstring>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

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

    constexpr stbir_filter filter2stbir(resize_filter f)
    {
        switch (f)
        {
        case resize_filter::BOX:
            return STBIR_FILTER_BOX;
        case resize_filter::CATMULL_ROM:
            return STBIR_FILTER_CATMULLROM;
        case resize_filter::CUBIC_SPLINE:
            return STBIR_FILTER_CUBICBSPLINE;
        case resize_filter::MITCHELL:
            return STBIR_FILTER_MITCHELL;
        case resize_filter::TRIANGLE:
            return STBIR_FILTER_TRIANGLE;
        case resize_filter::DEFAULT:
        default:
            return STBIR_FILTER_DEFAULT;
        }
    }

    constexpr int get_alpha_channel_index(image_format fmt)
    {
        switch (fmt)
        {
        case image_format::ABGR:
            return 0;
        case image_format::BGRA:
        case image_format::RGBA:
            return 3;
        default:
            return -1;
        }
    }

    image::image(size_t width, size_t height, image_format fmt)
        : image_data(width, height, fmt)
    {
        assert(width > 0 && height > 0);
    }

    image::image(const std::string& path, image_format fmt, image_load_mode load_mode)
    {
        assert(!path.empty());
        _format = fmt;
        if (load_mode == image_load_mode::IMAGE)
        {
            int dummy;
            int w, h;
            _data = stbi_load(path.c_str(), &w, &h, &dummy, channel_count());
            if (_data == nullptr)
            {
                throw std::logic_error(std::format("Unable to open image at '{}' ({})", path, stbi_failure_reason()));
            }

            _width = (size_t)w;
            _height = (size_t)h;
        }
        else // if (load_mode == image_load_mode::RAW)
        {
            std::optional<std::vector<uint8_t>> data = ien::read_file_binary(path);
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

            ien::deserializer deserializer(data->data(), data->size());            
			for(size_t i = 0; i < IEN_RAW_TAGGED_SIGNATURE.size(); ++i)
			{
				signature.push_back(deserializer.deserialize<char>());
			}

			if(signature != IEN_RAW_TAGGED_SIGNATURE)
			{
				throw std::logic_error("Raw tagged image file signature mismatch");
			}
			width = deserializer.deserialize<uint32_t>();
			height = deserializer.deserialize<uint32_t>();
			format = deserializer.deserialize<uint8_t>();

			if(width == 0 || height == 0)
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

    image::image(const unsigned char* data, size_t width, size_t height, image_format format)
        : image_data(width, height, format)
    {
        assert(data != nullptr);
        assert(width > 0 && height > 0);

        std::memcpy(_data, data, width * height * channel_count());
    }

    image image::resize(size_t width, size_t height, resize_filter filter) const
    {
        assert(width > 0 && height > 0);

        image result(width, height, _format);
        const int ok = stbir_resize_uint8_generic(
            _data,
            (int)_width,
            (int)_height,
            0,
            result._data,
            width,
            height,
            0,
            (int)channel_count(),
            get_alpha_channel_index(_format),
            0,
            STBIR_EDGE_WRAP,
            filter2stbir(filter),
            STBIR_COLORSPACE_SRGB,
            nullptr
        );

        if (ok == 0)
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
        const uint32_t width = (uint32_t)_width;
        const uint32_t height = (uint32_t)_height;
        const uint8_t fmt = static_cast<uint8_t>(format());

        ien::serializer serializer;
        serializer.serialize_buffer(IEN_RAW_TAGGED_SIGNATURE.data(), IEN_RAW_TAGGED_SIGNATURE.size());
        serializer.serialize(width);
        serializer.serialize(height);
        serializer.serialize(fmt);
        serializer.serialize(std::span(_data, size()));

        ien::write_file_binary(path, serializer.data());
    }

    void image::flip_axis_y()
    {
        image_data::flip_axis_y();
    }

    float image::absolute_difference(const image& other) const
    {
        return image_data::absolute_difference(other);
    }

    ien::image image::copy_rect(size_t x, size_t y, size_t w, size_t h) const
    {
        ien::image result(w, h, _format);

        for(size_t py = 0; py < h; ++py)
        {
            uint8_t* row_dst_ptr = result.data() + (py * image_format_channels(_format) * w);
            const uint8_t* row_src_ptr = data() + (py * image_format_channels(_format) * width()) + (x * image_format_channels(_format));
            std::memcpy(row_dst_ptr, row_src_ptr, (py * image_format_channels(_format) * w));
        }
        return result;
    }
} // namespace ien