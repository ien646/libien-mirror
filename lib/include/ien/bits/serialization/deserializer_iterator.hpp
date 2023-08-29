#pragma once

#include <cstddef>
#include <cstdint>

class deserializer_iterator
{
private:
    const uint8_t* _data;
    size_t _len;
    size_t _pos = 0;

public:
    deserializer_iterator(const uint8_t* data, size_t len)
        : _data(data)
        , _len(len)
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
};