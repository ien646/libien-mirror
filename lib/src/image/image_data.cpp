#include "ien/image/image.hpp"
#include "ien/image/image_format.hpp"
#include <ien/image/image_data.hpp>

#include <cassert>
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <utility>

namespace ien
{
    image_data::image_data(size_t w, size_t h, image_format fmt)
        : _width(w)
        , _height(h)
        , _format(fmt)
    {
        assert(w > 0 && h > 0);
        size_t alloc_sz = w * h * channel_count();
        _data = (uint8_t*)malloc(alloc_sz);
        if(_data == nullptr)
        {
            throw std::bad_alloc();
        }
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

    void image_data::cast_format(image_format target_format)
    {
        if(channel_count() != image_format_channels(target_format))
        {
            throw std::logic_error("Unable to cast image format with different number of channels");
        }

        if(target_format == _format || channel_count() < 3)
        {
            // Nothing to do here
            return;
        }

        if(channel_count() == 3)
        {
            shuffle({{2,1,0}}); //bgr <-> rgb
        }
        else if(channel_count() == 4)
        {
            if(_format == image_format::RGBA)
            {
                if(target_format == image_format::ABGR)
                { shuffle({{3,2,1,0}}); }
                else if(target_format == image_format::BGRA)
                { shuffle({{2,1,0,3}}); }
            }
            else if(_format == image_format::ABGR)
            {
                if(target_format == image_format::RGBA)
                { shuffle({{3,2,1,0}}); }
                else if(target_format == image_format::BGRA)
                { shuffle({{1,2,3,0}}); }
            }
            else if(_format == image_format::BGRA)
            {
                if(target_format == image_format::RGBA)
                { shuffle({{2,1,0,3}}); }
                else if(target_format == image_format::ABGR)
                { shuffle({{1,2,3,0}}); }
            }
        }
    }

    void image_data::shuffle(const image_shuffle& op)
    {
        const size_t pixels = pixel_count();
        const size_t channels = channel_count();
		if(channels == 1)
		{
			return;
		}

		const auto shuffle_index_std = [&](size_t pixel_index)
		{
			uint8_t* pxptr = _data + (pixel_index * channels);
			std::array<uint8_t, 4> temp;
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