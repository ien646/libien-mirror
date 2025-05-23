#pragma once

#include <bit>
#include <ien/bits/serialization/serializer_inserter.hpp>
#include <ien/lang_utils.hpp>

#include <cassert>
#include <limits>
#include <span>
#include <type_traits>

#ifndef IEN_SERIALIZE_CONTAINER_SIZE_T
    #define IEN_SERIALIZE_CONTAINER_SIZE_T uint32_t
#endif

namespace ien
{
    template <typename T>
    struct value_serializer
    {
        static void serialize(const T& t, serializer_inserter& inserter) = delete;
    };

    template <concepts::Integral T>
    struct value_serializer<T>
    {
        static void serialize(const T& v, serializer_inserter& inserter)
        {
            for (size_t i = 0; i < sizeof(T); ++i)
            {
                inserter += static_cast<std::byte>(v >> ((sizeof(T) - (i + 1)) * 8));
            }
        }
    };

    template <>
    struct value_serializer<std::byte>
    {
        static void serialize(const std::byte& v, serializer_inserter& inserter)
        {
            inserter += v;
        }
    };

    namespace detail
    {
        template <typename TContainer>
        static void serialize_container(const TContainer& container, serializer_inserter& inserter)
        {
            assert(container.size() <= std::numeric_limits<IEN_SERIALIZE_CONTAINER_SIZE_T>::max());

            ien::value_serializer<IEN_SERIALIZE_CONTAINER_SIZE_T>::serialize(
                static_cast<IEN_SERIALIZE_CONTAINER_SIZE_T> (container.size()),
                inserter
            );

            ien::value_serializer<typename TContainer::value_type> serializer;
            for (size_t i = 0; i < container.size(); ++i)
            {
                serializer.serialize(container[i], inserter);
            }
        }
    } // namespace detail

    template <concepts::FloatingPoint T>
    struct value_serializer<T>
    {
        static void serialize(const T& v, serializer_inserter& inserter)
        {
            using integral_type = std::conditional_t<std::is_same_v<float, T>, uint32_t, uint64_t>;
            ien::value_serializer<integral_type>::serialize(std::bit_cast<integral_type>(v), inserter);
        }
    };

    template <concepts::Enum T>
    struct value_serializer<T>
    {
        static void serialize(const T& t, serializer_inserter& inserter)
        {
            using integral_type = std::underlying_type_t<T>;
            ien::value_serializer<integral_type>::serialize(t, inserter);
        }
    };

    template <typename T, size_t ArrayLength>
    struct value_serializer<std::array<T, ArrayLength>>
    {
        static void serialize(const std::array<T, ArrayLength>& t, serializer_inserter& inserter)
        {
            detail::serialize_container(t, inserter);
        }
    };

    template <typename T>
    struct value_serializer<std::vector<T>>
    {
        static void serialize(const std::vector<T>& t, serializer_inserter& inserter)
        {
            detail::serialize_container(t, inserter);
        }
    };

    template <concepts::AnyChar T>
    struct value_serializer<std::basic_string<T>>
    {
        static void serialize(const std::basic_string<T>& t, serializer_inserter& inserter)
        {
            detail::serialize_container(t, inserter);
        }
    };

    template <typename T>
    struct value_serializer<std::span<T>>
    {
        static void serialize(const std::span<T>& t, serializer_inserter& inserter)
        {
            detail::serialize_container(t, inserter);
        }
    };

    template <typename T, size_t Extent>
    struct value_serializer<std::span<T, Extent>>
    {
        static void serialize(const std::span<T, Extent>& t, serializer_inserter& inserter)
        {
            detail::serialize_container(t, inserter);
        }
    };
} // namespace ien