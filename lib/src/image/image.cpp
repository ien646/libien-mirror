#include "ien/image/image_data.hpp"
#include <ien/image/image.hpp>

#include <ien/alloc.hpp>
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
			uint8_t channels = 0;

			fd.read(signature.data(), sizeof(signature));
			if(signature != IEN_RAW_TAGGED_SIGNATURE)
			{
				throw std::logic_error(fmt::format("Invalid tagged raw image signature for image '{}'", path));
			}

			fd.read(&width, sizeof(width));
			fd.read(&height, sizeof(height));
			fd.read(&channels, sizeof(channels));

			_width = (size_t)width;
			_height = (size_t)height;
			_format = static_cast<image_format>(channels);

			size_t total_bytes = _width * _height * static_cast<size_t>(_format);
			_data = (uint8_t*)malloc(total_bytes);

			fd.read(_data, total_bytes, 1);
		}
	}

	image::image(const unsigned char* data, size_t width, size_t height, image_format format)
		: image_data(width, height, format)
	{
		assert(data != nullptr);
		assert(width > 0 && height > 0);

		std::memcpy(_data, data, width * height * (size_t)format);
	}

	image image::resize(size_t width, size_t height) const
	{
		assert(width > 0 && height > 0);
		image result(width, height, _format);
		stbir_resize_uint8(_data, (int)_width, (int)_height, 0, result._data, width, height, 0, channel_count());
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

	void image::write_to_file_raw_tagged(const std::string& path) const
	{
		assert(!path.empty());
		uint32_t width = (uint32_t)_width;
		uint32_t height = (uint32_t)_height;
		uint8_t channels = channel_count();

		std::vector<uint8_t> binary;

		size_t offset = 0;
		size_t header_size = IEN_RAW_TAGGED_SIGNATURE.size() + sizeof(width) + sizeof(height) + sizeof(channels);
		size_t image_size = size();
		binary.resize(header_size + image_size);

		memcpy(binary.data() + offset, IEN_RAW_TAGGED_SIGNATURE.data(), IEN_RAW_TAGGED_SIGNATURE.size());
		offset += sizeof(IEN_RAW_TAGGED_SIGNATURE.size());

		memcpy(binary.data() + offset, reinterpret_cast<char*>(&width), sizeof(width));
		offset += sizeof(width);

		memcpy(binary.data() + offset, reinterpret_cast<char*>(&height), sizeof(height));
		offset += sizeof(height);

		memcpy(binary.data() + offset, reinterpret_cast<char*>(&channels), sizeof(channels));
		offset += sizeof(channels);

		// Copy image data into binary
		memcpy(binary.data() + offset, _data, image_size);

		ien::write_file_binary(path, binary);
	}

	void image::shuffle(const image_shuffle& op)
	{
		const size_t pixels = pixel_count();
		const size_t channels = static_cast<size_t>(_format);

		if(channels == 1)
		{
			return;
		}

		const auto shuffle_index_std = [&](size_t pixel_index)
		{
			uint8_t* pxptr = _data + (pixel_index * channels);
			std::array<uint8_t, 4> temp;
			for(size_t i = 0; i < channels; ++i)
			{
				temp[i] = pxptr[op.indices[i]];
			}

			for(size_t i = 0; i < channels; ++i)
			{
				pxptr[i] = temp[i];
			}
		};

		for(size_t i = 0; i < pixels; ++i)
		{
			shuffle_index_std(i);
		}
	}
}