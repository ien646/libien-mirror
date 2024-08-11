#pragma once

#include <stdexcept>
namespace ien
{
	enum class image_format
	{
		R,
		RG,
		RGB,
		RGBA,
	};

	inline constexpr unsigned char image_format_channels(image_format format)
	{
		switch(format)
		{
			case image_format::R:
				return 1;
			case image_format::RG:
				return 2;
			case image_format::RGB:
				return 3;
			case image_format::RGBA:
				return 4;
			default:
				throw std::invalid_argument("Invalid image_format");
		}
	}
}