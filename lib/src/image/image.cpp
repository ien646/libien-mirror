#include <ien/image/image.hpp>

#include <ien/io_utils.hpp>

#define STBI_WINDOWS_UTF8
#define STBIW_WINDOWS_UTF8
#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>

#include <array>
#include <filesystem>
#include <stdexcept>

namespace ien
{
	image::image(int width, int height, image_format fmt)
		: _format(fmt)
	{
		size_t total_bytes = (size_t)width * height * channel_count();

		// Using malloc() instead of new[] so that the deallocation interface matches stbi's
		_data = reinterpret_cast<unsigned char*>(malloc(total_bytes));

		_width = width;
		_height = height;
		_format = fmt;
	}

	image::image(const std::string& path, image_format fmt, image_load_mode load_mode)
		: _format(fmt)
	{
		if(!std::filesystem::exists(path))
		{
			throw std::invalid_argument("Image path not found");
		}

		if (load_mode == image_load_mode::IMAGE)
		{
			int dummy;
			int w, h;

			_data = stbi_load(path.c_str(), &w, &h, &dummy, channel_count());
			if (_data == nullptr)
			{
				throw std::logic_error("Unable to open image");
			}
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

			std::array<uint32_t, 3> header = {};
			fd.read(header.data(), 3, sizeof(uint32_t));

			_width = header[0];
			_height = header[1];
			_format = static_cast<image_format>(header[2]);

			size_t total_bytes = _width * _height * static_cast<size_t>(_format);
			_data = reinterpret_cast<unsigned char*>(malloc(total_bytes));

			fd.read(_data, total_bytes, 1);
		}
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
			free(_data);
		}
	}

	image image::resize(int width, int height)
	{
		image result(width, height, _format);
		stbir_resize_uint8(_data, (int)_width, (int)_height, 0, result._data, width, height, 0, channel_count());
		return result;
	}

	image image::extract_channel_image(size_t channel_index)
	{
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

    void image::write_to_file_png(const std::string& path, int compression)
    {
		int aux = stbi_write_png_compression_level;
		stbi_write_png_compression_level = compression;
        stbi_write_png(path.c_str(), (int)_width, (int)_height, channel_count(), _data, (int)(_width * channel_count()));
		stbi_write_png_compression_level = aux;
    }

	void image::write_to_file_bmp(const std::string& path)
	{
		stbi_write_bmp(path.c_str(), (int)_width, (int)_height, channel_count(), _data);
	}
}