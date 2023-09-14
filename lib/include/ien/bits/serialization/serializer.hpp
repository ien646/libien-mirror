#pragma once

#include <ien/bits/serialization/serializer_inserter.hpp>
#include <ien/bits/serialization/value_serializers.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

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

        template <typename T>
        void serialize_buffer(const T* ptr, size_t len)
        {
            for (size_t i = 0; i < len; ++i)
            {
                ien::value_serializer<T>{}.serialize(ptr[i], _inserter);
            }
        }

        inline const std::vector<uint8_t>& data() const { return _data; }
    };
} // namespace ien