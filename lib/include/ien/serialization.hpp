#pragma once

#include <ien/lang_utils.hpp>

#include <ien/bits/serialization/deserializer_iterator.hpp>
#include <ien/bits/serialization/serializer_inserter.hpp>
#include <ien/bits/serialization/value_deserializers.hpp>
#include <ien/bits/serialization/value_serializers.hpp>

#include <cstdint>

namespace ien
{
    class serializer
    {
    private:
        std::vector<uint8_t> _data;
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

        inline const std::vector<uint8_t>& data() const { return _data; }
    };



    template <typename TSourceIterator>
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

        template<typename T>
        T deserialize()
        {
            return ien::value_deserializer<T>{}.deserialize(_iterator);
        }
    };
} // namespace ien