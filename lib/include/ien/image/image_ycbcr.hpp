#pragma once

#include <ien/image/image.hpp>

namespace ien
{
	class image_ycbcr
		: public image
	{
	public:
		explicit image_ycbcr(const std::string& path);

	private:
		void convert_format();
	};
}