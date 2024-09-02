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
        std::vector<std::byte> _data;
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
        void serialize_buffer(std::span<const T> buff)
        {
            for (size_t i = 0; i < buff.size(); ++i)
            {
                ien::value_serializer<T>{}.serialize(buff[i], _inserter);
            }
        }

        const std::vector<std::byte>& data() const { return _data; }

        std::vector<std::byte> release_data() { return std::move(_data); }
    };
} // namespace ien