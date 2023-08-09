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

	enum class resize_filter
	{
		DEFAULT,
		BOX,
		CATMULL_ROM,
		CUBIC_SPLINE,
		MITCHELL,
		TRIANGLE
	};

	class image
		: public image_data
	{
	public:
		image(size_t width, size_t height, image_format = image_format::RGBA);
		explicit image(const std::string& path, image_format fmt = image_format::RGBA, image_load_mode load_mode = image_load_mode::IMAGE);
		image(const unsigned char* data, size_t width, size_t height, image_format format);
		image(const image& cp_src) = delete;
		image(image&& mv_src) = default;
		image& operator=(image&& mvsrc) = default;

        [[nodiscard]] image resize(size_t width, size_t height, resize_filter = resize_filter::DEFAULT) const;

		void write_to_file_png(const std::string& path, int compression = 8) const;
		void write_to_file_bmp(const std::string& path) const;
		void write_to_file_tga(const std::string& path) const;
		void write_to_file_jpg(const std::string& path, int quality = 100) const;
		void write_to_file_raw_tagged(const std::string& path) const;

		void flip_axis_y();

		float absolute_difference(const image& other) const;
	};
}