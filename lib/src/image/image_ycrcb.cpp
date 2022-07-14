#include <ien/image/image_ycrcb.hpp>

namespace ien
{
	image_ycrcb::image_ycrcb(const std::string& path)
		: image(path, image_format::RGB)
	{
		convert_format();
	}

	void image_ycrcb::convert_format()
	{
		for (size_t i = 0; i < size(); i += 3)
		{
			const unsigned char r = _data[i + 0];
			const unsigned char g = _data[i + 1];
			const unsigned char b = _data[i + 2];

			// JFIF modified Rec.601 for full range y/cb/cr
            const float y = (0.299F * r) + (0.587F * g) + (0.0114F * b);
            const float cb = 128.0F - (0.168736F * r) - (0.331264F * g) + (0.5F * b);
            const float cr = 128.0F + (0.5F * r) - (0.418688F * g) + (0.081312F * b);

            _data[i + 0] = static_cast<unsigned char>(y);
            _data[i + 1] = static_cast<unsigned char>(cb);
            _data[i + 2] = static_cast<unsigned char>(cr);
		}
	}
}