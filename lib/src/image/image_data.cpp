#include <ien/image/image_data.hpp>

#include <cstdlib>
#include <utility>

namespace ien
{
    image_data::image_data(size_t w, size_t h, image_format fmt)
        : _width(w)
        , _height(h)
        , _format(fmt)
    {
        _data = (uint8_t*)malloc(w * h * (size_t)fmt);
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
}