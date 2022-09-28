#pragma once

#include <ien/image/image_format.hpp>

#include <string>
#include <vector>

namespace ien
{
	enum class image_load_mode
	{
		IMAGE,
		RAW
	};

	class image
	{
	protected:
		unsigned char* _data = nullptr;
		size_t _width = 0;
		size_t _height = 0;
		image_format _format = image_format::RGBA;

	public:
		image(int width, int height, image_format = image_format::RGBA);
		image(const std::string& path, image_format fmt = image_format::RGBA, image_load_mode load_mode = image_load_mode::IMAGE);
		image(const unsigned char* data, int width, int height, image_format format);
		image(const image& cp_src) = delete;
		image(image&& mv_src) noexcept;
		~image();

		inline size_t width() const { return _width; }
		inline size_t height() const { return _height; }

		inline size_t pixel_count() const { return (size_t)_width * _height; }
		inline unsigned char channel_count() const { return static_cast<unsigned char>(_format); }
		inline size_t size() const { return pixel_count() * channel_count(); }

		inline unsigned char* data() { return _data; }
		inline const unsigned char* data() const { return _data; }

		image resize(int width, int height);

		void write_to_file_png(const std::string& path, int compression = 8);
		void write_to_file_bmp(const std::string& path);
		void write_to_file_raw_tagged(const std::string& path);

		image extract_channel_image(size_t channel_index);
	};

}