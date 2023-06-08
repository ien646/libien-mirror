#include <ien/image/image_data.hpp>
#include <ien/image/image_format.hpp>
#include <ien/image/image.hpp>

#include <ien/io_utils.hpp>
#include <ien/platform.hpp>

#define STBI_WINDOWS_UTF8
#define STBIW_WINDOWS_UTF8

#include <fmt/format.h>

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <array>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace ien
{
	const std::string IEN_RAW_TAGGED_SIGNATURE = "IRIS";

	constexpr stbir_filter filter2stbir(resize_filter f)
	{
		switch(f)
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
		switch(fmt)
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
				throw std::logic_error(fmt::format("Unable to open image at '{}'", path));
			}

			_width = (size_t)w;
			_height = (size_t)h;
		}
		else // if (load_mode == image_load_mode::RAW)
		{
			ien::unique_file_descriptor fd(path, "r");
			if (!fd)
			{
				throw std::logic_error(fmt::format("Unable to open image at '{}'", path));
			}

			std::string signature(4, 0);
			uint32_t width = 0;
			uint32_t height = 0;
			uint8_t format = 0;

			fd.read(signature.data(), signature.size());
			if(signature != IEN_RAW_TAGGED_SIGNATURE)
			{
				throw std::logic_error(fmt::format("Invalid tagged raw image signature for image '{}'", path));
			}

			fd.read(&width, sizeof(width));
			fd.read(&height, sizeof(height));
			fd.read(&format, sizeof(format));

			_width = (size_t)width;
			_height = (size_t)height;
			_format = static_cast<image_format>(format);

			size_t total_bytes = _width * _height * channel_count();
			_data = (uint8_t*)malloc(total_bytes);

			fd.read(_data, total_bytes, 1);
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
		int ok = stbir_resize_uint8_generic(
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

		if(ok == 0) {
			throw std::logic_error("Failure resizing image!");
		}
		return result;
	}

    void image::write_to_file_png(const std::string& path, int compression) const
    {
		assert(!path.empty());
		assert(compression > 0);
		int aux = stbi_write_png_compression_level;
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
		uint32_t width = (uint32_t)_width;
		uint32_t height = (uint32_t)_height;
		uint8_t fmt = static_cast<uint8_t>(format());

		std::vector<uint8_t> binary;

		size_t offset = 0;
		size_t header_size = IEN_RAW_TAGGED_SIGNATURE.size() + sizeof(width) + sizeof(height) + sizeof(fmt);
		size_t image_size = size();
		binary.resize(header_size + image_size);

		memcpy(binary.data() + offset, IEN_RAW_TAGGED_SIGNATURE.data(), IEN_RAW_TAGGED_SIGNATURE.size());
		offset += sizeof(IEN_RAW_TAGGED_SIGNATURE.size());

		memcpy(binary.data() + offset, reinterpret_cast<char*>(&width), sizeof(width));
		offset += sizeof(width);

		memcpy(binary.data() + offset, reinterpret_cast<char*>(&height), sizeof(height));
		offset += sizeof(height);

		memcpy(binary.data() + offset, reinterpret_cast<char*>(&fmt), sizeof(fmt));
		offset += sizeof(fmt);

		// Copy image data into binary
		memcpy(binary.data() + offset, _data, image_size);

		ien::write_file_binary(path, binary);
	}

	void image::flip_axis_y()
	{
		const size_t row_size = _width * image_format_channels(_format);
		std::vector<uint8_t> temp_row(row_size, 0);
		for(size_t i = 0; i < _height / 2; ++i)
		{
			uint8_t* row_up = _data + (row_size * i);
			uint8_t* row_down = _data + (row_size * (_height - i - 1));
			std::memcpy(temp_row.data(), row_up, row_size);
			std::memcpy(row_up, row_down, row_size);
			std::memcpy(row_down, temp_row.data(), row_size);
		}
	}
}