#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>

class deserializer_iterator
{
private:
    const uint8_t* _data;
    size_t _len;
    size_t _pos;

public:
    deserializer_iterator(const uint8_t* data, size_t len)
        : _data(data)
        , _len(len)
        , _pos(0)
    {
    }

    deserializer_iterator& operator++()
    {
        ++_pos;
        return *this;
    }

    deserializer_iterator& operator++(int)
    {
        deserializer_iterator& result = *this;
        ++_pos;
        return result;
    }

    uint8_t operator*() { return _data[_pos]; }

    inline void advance(size_t pos) 
    { 
        _pos += pos; 
        if(pos >= _len)
        {
            throw std::logic_error("Attempt to advance deserializer_iterator past end of data");
        }
    }

    inline const uint8_t* data() const { return _data; }
    inline size_t length() const { return _len; }
    inline size_t position() const { return _pos; }
};