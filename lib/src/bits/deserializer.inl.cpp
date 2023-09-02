#include <ien/bits/serialization/deserializer.hpp>

#include <cstring>

namespace ien
{
    deserializer::deserializer(const void* data, size_t len)
        : _iterator(reinterpret_cast<const uint8_t*>(data), len)
    {
    }

    template <typename T>
    deserializer::deserializer(const std::span<T>& data)
        : _iterator(reinterpret_cast<const uint8_t*>(data.data()), data.size())
    {
    }

    template <typename T>
    T deserializer::deserialize()
    {
        return ien::value_deserializer<T>{}.deserialize(_iterator);
    }

    template <typename T, bool Advance>
    void deserializer::deserialize_into_buffer(T* dst, size_t len)
    {
        size_t bytes = len * sizeof(T);
        if ((_iterator.position() + bytes) > length())
        {
            throw std::logic_error("Attempt to deserialize out of bounds data");
        }

        if constexpr (Advance)
        {
            for (size_t i = 0; i < len; ++i)
            {
                dst[i] = ien::value_deserializer<T>{}.deserialize(_iterator);
            }
        }
        else
        {
            deserializer_iterator it = _iterator;
            for (size_t i = 0; i < len; ++i)
            {
                dst[i] = ien::value_deserializer<T>{}.deserialize(it);
            }
        }
    }

    void deserializer::deserialize_into_buffer_uint8(uint8_t* dst, size_t len)
    {        
        if ((_iterator.position() + len) > length())
        {
            throw std::logic_error("Attempt to deserialize out of bounds data");
        }

        const uint8_t* data = _iterator.data();
        for (size_t i = 0; i < len; ++i)
        {
            std::memcpy(dst, data, len);
        }
    }

    template <>
    void deserializer::deserialize_into_buffer<uint8_t, true>(uint8_t* dst, size_t len)
    {
        deserialize_into_buffer_uint8(dst, len);
        _iterator.advance(len);
    }

    template <>
    void deserializer::deserialize_into_buffer<uint8_t, false>(uint8_t* dst, size_t len)
    {
        deserialize_into_buffer_uint8(dst, len);
    }
} // namespace ien