#pragma once

#include <bit>
#include <ien/lang_utils.hpp>
#include <ien/bits/serialization/deserializer_iterator.hpp>
#include <string>
#include <type_traits>
#include <span>
#include <vector>

namespace ien
{ 
    using SERIALIZE_CONTAINER_SIZE_T = uint32_t;

    template <typename T>
    struct value_deserializer
    {
        T deserialize(deserializer_iterator& iterator) const = delete;
    };

    template<concepts::Integral T>
    struct value_deserializer<T>
    {
        T deserialize(deserializer_iterator& iterator)
        {
            T result = 0;
            for(size_t i = 0; i < sizeof(T); ++i)
            {
                result |= (static_cast<T>(*iterator)) << ((sizeof(T) - (i + 1)) * 8);
                ++iterator;
            }
            return result;
        }
    };

    namespace detail
    {
        template<typename TContainer>
        TContainer deserialize_container(deserializer_iterator& iterator)
        {
            auto len = ien::value_deserializer<SERIALIZE_CONTAINER_SIZE_T>{}.deserialize(iterator);

            TContainer result;
            result.reserve(len / sizeof(typename TContainer::value_type));

            for(size_t i = 0; i < len; ++i)
            {
                result.push_back(ien::value_deserializer<typename TContainer::value_type>{}.deserialize(iterator));
            }
            return result;
        }
    }

    template<concepts::FloatingPoint T>
    struct value_deserializer<T>
    {
        T deserialize(deserializer_iterator& iterator)
        {
            using integral_type = std::conditional_t<std::is_same_v<float, T>, uint32_t, uint64_t>;
            integral_type intval = ien::value_deserializer<integral_type>{}.deserialize(iterator);
            return std::bit_cast<T>(intval);
        }
    };

    template<concepts::Enum T>
    struct value_deserializer<T>
    {
        T deserialize(deserializer_iterator& iterator)
        {
            using integral_type = std::underlying_type_t<T>;
            integral_type intval = ien::value_deserializer<integral_type>{}.deserialize(iterator);
            return std::bit_cast<T>(intval);
        }
    };

    template<typename T, size_t ArrayLength>
    struct value_deserializer<std::array<T, ArrayLength>>
    {
        std::array<T, ArrayLength> deserialize(deserializer_iterator& iterator)
        {
            return detail::deserialize_container<std::array<T, ArrayLength>>(iterator); 
        }
    };

    template<typename T>
    struct value_deserializer<std::vector<T>>
    {
        std::vector<T> deserialize(deserializer_iterator& iterator)
        {
            return detail::deserialize_container<std::vector<T>>(iterator); 
        }
    };

    template<typename T>
    struct value_deserializer<std::basic_string<T>>
    {
        std::basic_string<T> deserialize(deserializer_iterator& iterator)
        {
            return detail::deserialize_container<std::basic_string<T>>(iterator);
        }
    };
}