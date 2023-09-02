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
        void serialize(const T& e);

        template <typename T>
        void serialize_buffer(const T* ptr, size_t len);

        inline const std::vector<uint8_t>& data() const { return _data; }
    };
} // namespace ien