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
        template<typename T>
        explicit deserializer(std::span<T> data)
            : _iterator(data)
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
            if ((_iterator.position() + (len * sizeof(T))) > length())
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

        template <bool Advance = true>
        void deserialize_into_buffer_of_bytes(std::byte* dst, size_t len)
        {
            if ((_iterator.position() + len) > length())
            {
                throw std::logic_error("Attempt to deserialize out of bounds data");
            }

            const std::byte* data = _iterator.data() + _iterator.position();
            for (size_t i = 0; i < len; ++i)
            {
                std::memcpy(dst, data, len);
            }

            if constexpr (Advance)
            {
                _iterator.advance(len);
            }
        }

        inline void advance(size_t bytes) { _iterator.advance(bytes); }

        inline const std::byte* data() const { return _iterator.data(); }
        inline size_t length() const { return _iterator.length(); }
        inline size_t position() const { return _iterator.position(); }
    };
} // namespace ien