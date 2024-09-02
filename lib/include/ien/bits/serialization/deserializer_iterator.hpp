#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

class deserializer_iterator
{
private:
    std::span<const std::byte> _data;
    size_t _pos = 0;

public:
    template <typename T>
    explicit deserializer_iterator(std::span<T> data)
        : _data(std::span<const std::byte>{ reinterpret_cast<const std::byte*>(data.data()), data.size_bytes() })
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

    std::byte operator*() const { return _data[_pos]; }

    inline void advance(size_t pos)
    {
        _pos += pos;
        if (pos >= _data.size())
        {
            throw std::logic_error("Attempt to advance deserializer_iterator past end of data");
        }
    }

    inline const std::byte* data() const { return _data.data(); }
    inline size_t length() const { return _data.size(); }
    inline size_t position() const { return _pos; }
};