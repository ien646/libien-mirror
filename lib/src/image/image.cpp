#include <ien/image/image.hpp>

#include <ien/alloc.hpp>
#include <ien/assert.hpp>
#include <ien/io_utils.hpp>
#include <ien/platform.hpp>

#define STBI_WINDOWS_UTF8
#define STBIW_WINDOWS_UTF8

#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#if defined(IEN_ARCH_X86) || defined(IEN_ARCH_X86_64)
#include <xmmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>
#endif

namespace ien
{
	image::image(int width, int height, image_format fmt)
		: _format(fmt)
		, _width(width)
		, _height(height)
	{
		IEN_ASSERT(width > 0 && height > 0);
		size_t total_bytes = (size_t)width * height * channel_count();

		// Using malloc() instead of new[] so that the deallocation interface matches stbi's
		_data = ien::aligned_alloc(total_bytes, IEN_SSE_ALIGNMENT);
	}

	image::image(const std::string& path, image_format fmt, image_load_mode load_mode)
		: _format(fmt)
	{
		IEN_ASSERT(!path.empty());
		if (load_mode == image_load_mode::IMAGE)
		{
			int dummy;
			int w, h;

			_data = stbi_load(path.c_str(), &w, &h, &dummy, channel_count());
			if (_data == nullptr)
			{
				throw std::logic_error("Unable to open image");
			}
			uint8_t* aligned_data = ien::aligned_alloc(w * h * channel_count(), IEN_SSE_ALIGNMENT);
			std::memcpy(aligned_data, _data, w * h * channel_count());
			free(_data);
			_data = aligned_data;

			_width = (size_t)w;
			_height = (size_t)h;
		}
		else // if (load_mode == image_load_mode::RAW)
		{
			ien::unique_file_descriptor fd(path, "r");
			if (!fd)
			{
				throw std::logic_error("Unable to open image");
			}

			char signature[4] = { 0, 0, 0, 0 };
			uint32_t width = 0;
			uint32_t height = 0;
			uint8_t channels = 0;

			fd.read(signature, sizeof(signature));
			if(strncmp(signature, "IRIS", 4) != 0)
			{
				throw std::logic_error("Invalid tagged raw image signature");
			}

			fd.read(&width, sizeof(width));
			fd.read(&height, sizeof(height));
			fd.read(&channels, sizeof(channels));

			_width = (size_t)width;
			_height = (size_t)height;
			_format = static_cast<image_format>(channels);

			size_t total_bytes = _width * _height * static_cast<size_t>(_format);
			_data = ien::aligned_alloc(total_bytes, IEN_SSE_ALIGNMENT);

			fd.read(_data, total_bytes, 1);
		}
	}

	image::image(const unsigned char* data, int width, int height, image_format format)
		: _format(format)
		, _width(width)
		, _height(height)
	{
		IEN_ASSERT(data != nullptr);
		IEN_ASSERT(width > 0 && height > 0);

		size_t alloc_sz = width * height * static_cast<int>(format);
		_data = ien::aligned_alloc(alloc_sz, IEN_SSE_ALIGNMENT);
		std::memcpy(_data, data, alloc_sz);
	}

	image::image(image&& mv_src) noexcept
	{
		_data = mv_src._data;
		_width = mv_src._width;
		_height = mv_src._height;
		_format = mv_src._format;

		mv_src._data = nullptr;
		mv_src._width = 0;
		mv_src._height = 0;
	}

	image::~image()
	{
		if (_data != nullptr)
		{
			ien::aligned_free(_data);
		}
	}

	image image::resize(int width, int height) const
	{
		IEN_ASSERT(width > 0 && height > 0);
		image result(width, height, _format);
		stbir_resize_uint8(_data, (int)_width, (int)_height, 0, result._data, width, height, 0, channel_count());
		return result;
	}

	image image::extract_channel_image(size_t channel_index) const
	{
		IEN_ASSERT(channel_index <= 4);
		if (channel_index >= channel_count())
		{
			throw std::out_of_range("Invalid image channel index");
		}

		image result((int)_width, (int)_height, image_format::R);
		const size_t channels = channel_count();
		const size_t pixels = pixel_count();
		for (size_t i = 0, j = channel_index; i < pixels; ++i, j += channels)
		{
			result._data[i] = _data[j];
		}
		return result;
	}

    void image::write_to_file_png(const std::string& path, int compression) const
    {
		IEN_ASSERT(!path.empty());
		IEN_ASSERT(compression > 0);
		int aux = stbi_write_png_compression_level;
		stbi_write_png_compression_level = compression;
        stbi_write_png(path.c_str(), (int)_width, (int)_height, channel_count(), _data, (int)(_width * channel_count()));
		stbi_write_png_compression_level = aux;
    }

	void image::write_to_file_bmp(const std::string& path) const
	{
		IEN_ASSERT(!path.empty());
		stbi_write_bmp(path.c_str(), (int)_width, (int)_height, channel_count(), _data);
	}

	void image::write_to_file_raw_tagged(const std::string& path) const
	{
		IEN_ASSERT(!path.empty());
		constexpr char signature[4] = { 'I', 'R', 'I', 'S' }; // Ien's Raw Image Signature
		uint32_t width = (uint32_t)_width;
		uint32_t height = (uint32_t)_height;
		uint8_t channels = channel_count();

		std::vector<uint8_t> binary;

		size_t offset = 0;
		size_t header_size = sizeof(signature) + sizeof(width) + sizeof(height) + sizeof(channels);
		size_t image_size = size();
		binary.resize(header_size + image_size);

		memcpy(binary.data() + offset, signature, sizeof(signature));
		offset += sizeof(signature);

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
			uint8_t temp[4];
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