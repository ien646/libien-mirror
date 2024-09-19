#pragma once

#include <ien/platform.hpp>

#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <deque>
#include <vector>

namespace ien
{
    /* -- Template Utils -- */

    template <typename T, typename... TArgs>
    struct is_one_of
    {
        static constexpr bool value = (std::is_same_v<T, TArgs> || ...);
    };

    template <typename T, typename... TArgs>
    constexpr bool is_one_of_v = is_one_of<T, TArgs...>::value;

    template <typename TTo, typename... TFrom>
    struct all_convertible_to
    {
        static constexpr bool value = (std::is_convertible_v<TTo, TFrom> && ...);
    };

    template <typename TTo, typename... TFrom>
    constexpr bool all_convertible_to_v = all_convertible_to<TTo, TFrom...>::value;

    template <typename>
    struct template_params
    {
    };

    template <template <typename... Args> typename T, typename... Args>
    struct template_params<T<Args...>>
    {
    };

    template <typename T>
    using template_params_t = typename template_params<T>::type;

    template <typename, typename>
    struct is_type_in_template
    {
    };

    template <typename T, template <typename...> typename V, typename... Args>
    struct is_type_in_template<T, V<Args...>>
    {
        static constexpr bool value = is_one_of_v<T, Args...>;
    };

    template <typename T, typename V>
    constexpr bool is_type_in_template_v = is_type_in_template<T, V>::value;

    template <typename T>
    struct raw_str_char
    {
        using type = typename std::remove_const_t<std::remove_pointer_t<std::decay_t<T>>>;
    };

    template <typename T>
    using raw_str_char_t = typename raw_str_char<T>::type;

    /* -- Concepts -- */

    namespace concepts
    {
        template <typename T>
        concept Pointer = std::is_pointer_v<std::decay_t<T>>;

        template <typename T>
        concept CArray = std::is_array_v<T>;

        template <typename T>
        concept CChar = is_one_of_v<std::remove_const_t<T>, char, wchar_t>;

        template <typename T>
        concept UnicodeChar = is_one_of_v<std::remove_const_t<T>, char8_t, char16_t, char32_t>;

        template <typename T>
        concept AnyChar = CChar<T> || UnicodeChar<T>;

        template <typename T>
        concept Enum = std::is_enum_v<T>;

        template <typename T>
        concept Integral = std::is_integral_v<T>;

        template <typename T>
        concept SignedIntegral = std::is_signed_v<T> && std::is_integral_v<T>;

        template <typename T>
        concept UnsignedIntegral = std::is_unsigned_v<T> && std::is_integral_v<T>;

        template <typename T>
        concept FloatingPoint = std::is_floating_point_v<T>;

        template <typename T>
        concept Arithmetic = std::is_arithmetic_v<T>;

        template <typename T>
        concept RawCStr = (Pointer<T> || CArray<T>)&&CChar<raw_str_char_t<T>>;

        template <typename T>
        concept RawUnicodeStr = (Pointer<T> || CArray<T>)&&UnicodeChar<raw_str_char_t<T>>;

        template <typename T>
        concept RawAnyStr = RawCStr<T> || RawUnicodeStr<T>;

        template <typename T>
        concept StdCStr = is_one_of_v<std::remove_cvref_t<T>, std::string, std::wstring>;

        template <typename T>
        concept StdUnicodeStr = is_one_of_v<std::remove_cvref_t<T>, std::u8string, std::u16string, std::u32string>;

        template <typename T>
        concept StdAnyStr = StdCStr<T> || StdUnicodeStr<T>;

        template <typename T>
        concept StdCStrV = is_one_of_v<std::remove_cvref_t<T>, std::string_view, std::wstring_view>;

        template <typename T>
        concept StdUnicodeStrV =
            is_one_of_v<std::remove_cvref_t<T>, std::u8string_view, std::u16string_view, std::u32string_view>;

        template <typename T>
        concept StdAnyStrV = StdCStrV<T> || StdUnicodeStrV<T>;

        template <typename T>
        concept StdCStrStrV = StdCStr<T> || StdCStrV<T>;

        template <typename T>
        concept StdAnyStrStrV = StdAnyStr<T> || StdAnyStrV<T>;

        template <typename T>
        concept AnyStr = StdAnyStrStrV<T> || RawAnyStr<T>;

        template <typename T>
        concept AnyStrOrChar = AnyStr<T> || AnyChar<T>;

        template <typename T>
        concept IterableContainer = requires(T c) { c.begin(); };

        template <typename T>
        concept SizedContainer = IterableContainer<T> && requires(T c) { c.size(); };

        template <typename T>
        concept FlatContainer = requires(T c) {
            c.begin();
            c.end();
            c.data();
            c.size();
        };

        template <typename T>
        concept RandomAccessContainer = requires(T c) { c[0]; } || requires(T c) { c.at(0); };

        template <typename T>
        concept HasPopBack = requires(T t) { t.pop_back(); };

        template <typename T>
        concept HasPushBack = requires(T t) { typename std::remove_cvref_t<T>::value_type; } &&
                              requires(T t, typename std::remove_cvref_t<T>::value_type v) { t.push_back(v); };
    } // namespace concepts

    /* -- Miscellaneous -- */

    template <concepts::Pointer T>
    constexpr auto* data(T& val)
    {
        return val;
    }

    template <concepts::Pointer T>
    constexpr const auto* data(const T& val)
    {
        return val;
    }

    template <typename T>
        requires(requires(T& v) { v.data(); })
    constexpr auto* data(T& val)
    {
        return val.data();
    }

    /// @brief Obtain the first value in function arguments that has the same type as T
    template <typename T, typename... TArgs>
        requires(std::is_same_v<T, TArgs> || ...)
    constexpr T select_template(TArgs... args)
    {
        return std::get<T>(std::tuple<TArgs...>(args...));
    }

    template <concepts::Integral T>
    struct superior_integral
    {
    };
    template <>
    struct superior_integral<uint8_t>
    {
        using type = uint16_t;
    };
    template <>
    struct superior_integral<uint16_t>
    {
        using type = uint32_t;
    };
    template <>
    struct superior_integral<uint32_t>
    {
        using type = uint64_t;
    };
    template <>
    struct superior_integral<int8_t>
    {
        using type = int16_t;
    };
    template <>
    struct superior_integral<int16_t>
    {
        using type = int32_t;
    };
    template <>
    struct superior_integral<int32_t>
    {
        using type = int64_t;
    };

    template <concepts::Integral T>
    using superior_integral_t = typename superior_integral<T>::type;

    /// @brief Get the underlying character type of a string-like type T.
    /// Fallback specialization.
    template <typename T>
    struct underlying_char
    {
    };

    /// @brief Get the underlying character type of a string-like type T.
    /// Scalar character type specialization.
    template <concepts::AnyChar T>
    struct underlying_char<T>
    {
        using type = std::remove_const_t<T>;
    };

    /// @brief Get the underlying character type of a string-like type T.
    /// std::basic_string/basic_string_view type specialization.
    template <concepts::StdAnyStrStrV T>
    struct underlying_char<T>
    {
        using type = std::remove_const_t<typename T::value_type>;
    };

    /// @brief Get the underlying character type of a string-like type T.
    /// C-string (const char*/char[]) type specialization.
    template <concepts::RawAnyStr T>
    struct underlying_char<T>
    {
        using type = std::remove_const_t<raw_str_char_t<T>>;
    };

    /// @brief Get the underlying character type of a string-like type T.
    template <concepts::AnyStrOrChar T>
    using underlying_char_t = typename underlying_char<T>::type;

    /// @brief Obtain a data pointer for any string-like or character type
    template <concepts::StdAnyStrStrV T>
    constexpr underlying_char_t<T>* anystr_data(T& str)
    {
        return str.data();
    }

    /// @brief Obtain a data pointer for any string-like or character type
    template <concepts::StdAnyStrStrV T>
    constexpr const underlying_char_t<T>* anystr_data(const T& str)
    {
        return str.data();
    }

    /// @brief Obtain a data pointer for any string-like or character type
    template <concepts::AnyChar T>
    constexpr underlying_char_t<T>* anystr_data(T& str)
    {
        return &str;
    }

    /// @brief Obtain a data pointer for any string-like or character type
    template <concepts::AnyChar T>
    constexpr const underlying_char_t<T>* anystr_data(const T& str)
    {
        return &str;
    }

    /// @brief Obtain a data pointer for any string-like or character type
    template <concepts::RawAnyStr T>
    constexpr underlying_char_t<T>* anystr_data(T& str)
    {
        return &str[0];
    }

    /// @brief Obtain a data pointer for any string-like or character type
    template <concepts::RawAnyStr T>
    constexpr const underlying_char_t<T>* anystr_data(const T& str)
    {
        return &str[0];
    }

    /// @brief Obtain the length value for any string-like or character type
    template <concepts::AnyChar T>
    constexpr inline size_t anystr_length([[maybe_unused]] const T& str)
    {
        return 1;
    }

    /// @brief Obtain the length value for any string-like or character type
    template <concepts::StdAnyStrStrV T>
    constexpr inline size_t anystr_length(const T& str)
    {
        return str.size();
    }

    /// @brief Obtain the length value for any string-like or character type
    template <concepts::RawAnyStr T>
    constexpr inline size_t anystr_length(const T& str)
    {
        return std::basic_string_view<raw_str_char_t<T>>(str).size();
    }

    template <typename T>
    using container_value_type = typename std::remove_cvref_t<T>::value_type;
} // namespace ien