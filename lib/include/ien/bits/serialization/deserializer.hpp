#pragma once

#include <ien/bits/serialization/deserializer_iterator.hpp>
#include <ien/bits/serialization/value_deserializers.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>

namespace ien
{
    class deserializer
    {
    private:
        deserializer_iterator _iterator;

    public:
        deserializer(const void* data, size_t len)
            : _iterator(reinterpret_cast<const uint8_t*>(data), len)
        {
        }

        template <typename T>
        deserializer(const std::span<T>& data)
            : _iterator(reinterpret_cast<const uint8_t*>(data.data()), data.size())
        {
        }

        template <typename T>
        T deserialize()
        {
            return ien::value_deserializer<T>{}.deserialize(_iterator);
        }

        template <typename T, bool Advance = true>
        void deserialize_into_buffer(T* dst, size_t len)
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

        void deserialize_into_buffer_uint8(uint8_t* dst, size_t len)
        {
            if ((_iterator.position() + len) > length())
            {
                throw std::logic_error("Attempt to deserialize out of bounds data");
            }

            const uint8_t* data = _iterator.data() + _iterator.position();
            for (size_t i = 0; i < len; ++i)
            {
                std::memcpy(dst, data, len);
            }
        }

        inline void advance(size_t bytes) { _iterator.advance(bytes); }

        inline const uint8_t* data() const { return _iterator.data(); }
        inline size_t length() const { return _iterator.length(); }
        inline size_t position() const { return _iterator.position(); }
    };

    template <>
    inline void deserializer::deserialize_into_buffer<uint8_t, true>(uint8_t* dst, size_t len)
    {
        deserialize_into_buffer_uint8(dst, len);
        _iterator.advance(len);
    }

    template <>
    inline void deserializer::deserialize_into_buffer<uint8_t, false>(uint8_t* dst, size_t len)
    {
        deserialize_into_buffer_uint8(dst, len);
    }
} // namespace ien