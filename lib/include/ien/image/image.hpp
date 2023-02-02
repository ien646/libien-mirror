#pragma once

#include <ien/image/image_data.hpp>
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

	struct image_shuffle
	{
		std::array<uint8_t, 4> indices = {0, 1, 2, 3};
	};

	class image
		: public image_data
	{
	public:
		image(int width, int height, image_format = image_format::RGBA);
		explicit image(const std::string& path, image_format fmt = image_format::RGBA, image_load_mode load_mode = image_load_mode::IMAGE);
		image(const unsigned char* data, int width, int height, image_format format);
		image(const image& cp_src) = delete;
		image(image&& mv_src) = default;
		image& operator=(image&& mvsrc) = default;

		inline size_t width() const { return _width; }
		inline size_t height() const { return _height; }

		inline size_t pixel_count() const { return _width * _height; }
		inline unsigned char channel_count() const { return static_cast<unsigned char>(_format); }
		inline size_t size() const { return pixel_count() * channel_count(); }

		inline unsigned char* data() { return _data; }
		inline const unsigned char* data() const { return _data; }

        [[nodiscard]] image resize(int width, int height) const;

		void write_to_file_png(const std::string& path, int compression = 8) const;
		void write_to_file_bmp(const std::string& path) const;
		void write_to_file_tga(const std::string& path) const;
		void write_to_file_raw_tagged(const std::string& path) const;

        image extract_channel_image(size_t channel_index) const;

		void shuffle(const image_shuffle&);
	};
}