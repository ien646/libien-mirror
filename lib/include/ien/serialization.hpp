#pragma once

#include <ien/lang_utils.hpp>

#include <ien/bits/serialization/deserializer_iterator.hpp>
#include <ien/bits/serialization/serializer_inserter.hpp>
#include <ien/bits/serialization/value_deserializers.hpp>
#include <ien/bits/serialization/value_serializers.hpp>

#include <cstdint>
#include <stdexcept>

namespace ien
{
    class serializer
    {
    private:
        std::vector<uint8_t> _data;
        std::vector<uint32_t> _header;
        serializer_inserter _inserter;

    public:
        serializer()
            : _inserter(serializer_inserter(_data))
        {
        }

        template <typename T>
        void serialize(const T& e)
        {
            ien::value_serializer<T>().serialize(e, _inserter);
        }

        template<typename T>
        void serialize_buffer(const T* ptr, size_t len)
        {
            for(size_t i = 0; i < len; ++i)
            {
                ien::value_serializer<T>{}.serialize(ptr[i], _inserter);
            }
        }

        inline const std::vector<uint8_t>& data() const { return _data; }
    };

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
        explicit deserializer(const std::span<T>& data)
            : _iterator(reinterpret_cast<const uint8_t*>(data.data()), data.size())
        {
        }

        template <typename T>
        T deserialize()
        {
            return ien::value_deserializer<T>{}.deserialize(_iterator);
        }

        inline void advance(size_t bytes) { _iterator.advance(bytes); }

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
                    dst[i] = ien::value_deserializer<uint8_t>{}.deserialize(_iterator);
                }
            }
            else
            {
                deserializer_iterator it = _iterator;
                for (size_t i = 0; i < len; ++i)
                {
                    dst[i] = ien::value_deserializer<uint8_t>{}.deserialize(it);
                }
            }
        }

        inline const uint8_t* data() const { return _iterator.data(); }
        inline size_t length() const { return _iterator.length(); }
        inline size_t position() const { return _iterator.position(); }
    };
} // namespace ien