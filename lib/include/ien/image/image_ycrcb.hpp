#pragma once

#include <ien/image/image.hpp>

namespace ien
{
	class image_ycrcb
		: public image
	{
	public:
		explicit image_ycrcb(const std::string& path);

	private:
		void convert_format();
	};
}