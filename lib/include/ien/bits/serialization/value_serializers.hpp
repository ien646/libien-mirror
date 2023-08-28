#pragma once

#include <bit>
#include <ien/lang_utils.hpp>
#include <ien/bits/serialization/serializer_inserter.hpp>

#include <span>
#include <type_traits>

namespace ien
{
    // By default, ien::serialize only exists for types that explicitly implement overloads
    template<typename T>
    struct value_serializer
    {
        void serialize(const T& t, serializer_inserter& inserter) const = delete;
    };

    template<concepts::Integral T>
    struct value_serializer<T>
    {   
        void serialize(const T& v, serializer_inserter& inserter)
        {
            for(size_t i = 0; i < sizeof(T); ++i)
            {
                inserter += static_cast<uint8_t>(v >> ((sizeof(T) - (i + 1)) * 8));
            }
        }
    };

    template<concepts::FloatingPoint T>
    struct value_serializer<T>
    {   
        void serialize(const T& v, serializer_inserter& inserter)
        {
            using integral_type = std::conditional_t<std::is_same_v<float, T>, uint32_t, uint64_t>;
            ien::value_serializer<integral_type>{}.serialize(std::bit_cast<integral_type>(v), inserter);
        }
    };

    template<concepts::Enum T>
    struct value_serializer<T>
    {   
        void serialize(const T& t, serializer_inserter& inserter)
        {
            using integral_type = std::underlying_type_t<T>;
            ien::value_serializer<integral_type>{}.serialize(t, inserter);
        }
    };

    template<typename T>
    struct value_serializer<std::vector<T>>
    {   
        void serialize(const std::vector<T>& t, serializer_inserter& inserter)
        {
            ien::value_serializer<T> serializer;
            for(size_t i = 0; i < t.size(); ++i)
            {
                serializer.serialize(t[i], inserter);
            }
        }
    };

    template<concepts::AnyChar T>
    struct value_serializer<std::basic_string<T>>
    {   
        void serialize(const std::basic_string<T>& t, serializer_inserter& inserter)
        {
            ien::value_serializer<T> serializer;
            for(size_t i = 0; i < t.size(); ++i)
            {
                serializer.serialize(t[i], inserter);
            }
        }
    };

    template<typename T>
    struct value_serializer<std::span<T>>
    {   
        void serialize(const std::span<T>& t, serializer_inserter& inserter)
        {
            ien::value_serializer<T> serializer;
            for(size_t i = 0; i < t.size(); ++i)
            {
                serializer.serialize(t[i], inserter);
            }
        }
    };

    template<typename T, size_t Extent>
    struct value_serializer<std::span<T, Extent>>
    {   
        void serialize(const std::span<T, Extent>& t, serializer_inserter& inserter)
        {
            ien::value_serializer<T> serializer;
            for(size_t i = 0; i < t.size(); ++i)
            {
                serializer.serialize(t[i], inserter);
            }
        }
    };
}