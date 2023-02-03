#include "ien/image/image_format.hpp"
#include <ien/image/image_data.hpp>

#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <utility>

namespace ien
{
    image_data::image_data(size_t w, size_t h, image_format fmt)
        : _width(w)
        , _height(h)
        , _format(fmt)
    {
        _data = (uint8_t*)malloc(w * h * image_format_channels(fmt));
    }

    image_data::image_data(image_data&& mvsrc)
    {
        *this = std::move(mvsrc);
    }

    image_data::~image_data()
    {
        if(_data != nullptr)
        {
            free(_data);
        }
        _data = nullptr;
        _width = 0;
        _height = 0;
    }

    image_data& image_data::operator=(image_data&& mvsrc)
    {
        _width = mvsrc._width;
        _height = mvsrc._height;
        _data = mvsrc._data;
        _format = mvsrc._format;

        mvsrc._data = nullptr;
        return *this;
    }

    image_data image_data::extract_channel(size_t channel_index) const
    {
        assert(channel_index <= 4);
		if (channel_index >= channel_count())
		{
			throw std::out_of_range("Invalid image channel index");
		}

		image_data result(_width, _height, image_format::R);
		const size_t channels = channel_count();
		const size_t pixels = pixel_count();
		for (size_t i = 0, j = channel_index; i < pixels; ++i, j += channels)
		{
			result._data[i] = _data[j];
		}
		return result;
    }
}