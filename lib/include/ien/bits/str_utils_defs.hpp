#pragma once

#include <ien/lang_utils.hpp>
#include <ien/platform.hpp>

namespace ien::detail
{
    template<concepts::AnyStrOrChar T, concepts::AnyStrOrChar ... TOther>
    constexpr bool is_same_underlying_char_type =
        (std::is_same_v<underlying_char_t<T>, underlying_char_t<TOther>> && ...);

    template<concepts::AnyStrOrChar T>
    constexpr inline size_t str_length(const T& str)
    {
        if constexpr (concepts::AnyChar<T>)
        {
            return 1;
        }
        else if constexpr (concepts::StdAnyStrStrV<T>)
        {
            return str.size();
        }
        else if constexpr (concepts::RawAnyStr<T>)
        {
            return std::basic_string_view<raw_str_char_t<T>>(str).size();
        }
        IEN_HINT_UNREACHABLE();
        return 0;
    };

    template<concepts::AnyStrOrChar T>
    constexpr inline const underlying_char_t<T>* str_dataptr(const T& str)
    {
        constexpr bool x = concepts::AnyChar<const char*>;
        if constexpr (concepts::AnyChar<T>)
        {
            return &str;
        }
        else if constexpr (concepts::StdAnyStrStrV<T>)
        {
            return str.data();
        }
        else if constexpr (concepts::RawAnyStr<T>)
        {
            return &(str[0]);
        }
        IEN_HINT_UNREACHABLE();
        return nullptr;
    };

    template<concepts::AnyStr T>
    constexpr std::basic_string_view<underlying_char_t<T>> str_view_range(const T& str, size_t offset, size_t maxlen)
    {
        using char_type = underlying_char_t<T>;
        return std::basic_string_view<char_type>(
            anystr_data(str) + offset,
            std::min(str_length(str) - offset, maxlen)
        );
    }

    template<bool StrView, typename TChar>
    using str_or_strview = std::conditional_t<StrView, std::basic_string_view<TChar>, std::basic_string<TChar>>;

    template<concepts::AnyStr TStr, concepts::AnyStrOrChar TOcurrence>
        requires is_same_underlying_char_type<TStr, TOcurrence>
    constexpr std::vector<size_t> str_find_ocurrence_indices(const TStr& str, const TOcurrence& ocurrence)
    {
        const size_t ocurrence_len = str_length(ocurrence);
        std::vector<size_t> found_indices;
        size_t from_offset = 0;
        from_offset = str.find(ocurrence, from_offset);
        while (from_offset != str.npos)
        {
            found_indices.push_back(from_offset);
            from_offset = str.find(ocurrence, from_offset + ocurrence_len);
        }
        return found_indices;
    };
}