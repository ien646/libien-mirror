#pragma once

#include <ien/lang_utils.hpp>

#include <ien/bits/serialization/serializer_inserter.hpp>
#include <ien/bits/serialization/value_serializers.hpp>

#include <cstdint>
#include <iterator>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

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

        template<typename T>
        void serialize(const T& e)
        {
            ien::value_serializer<T>().serialize(e, _inserter);
        }

        inline const std::vector<uint8_t>& data() const { return _data; }
    };
}