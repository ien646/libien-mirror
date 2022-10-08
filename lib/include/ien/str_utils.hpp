#pragma once

#include <ien/lang_utils.hpp>
#include <ien/platform.hpp>

#include <ien/bits/str_utils_defs.hpp>
#include <ien/bits/str_split.hpp>

#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ien
{
    template<concepts::AnyStr T, concepts::AnyStrOrChar TDelim>
        requires detail::is_same_underlying_char_type<T, TDelim>
    constexpr std::vector<std::basic_string<underlying_char_t<T>>> str_split(
        const T& str,
        const TDelim& delim,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        return detail::str_split<false>(str, delim, offset, maxlen);
    }

    template<concepts::AnyStr T, concepts::AnyStrOrChar TDelim>
        requires detail::is_same_underlying_char_type<T, TDelim>
    constexpr std::vector<std::basic_string_view<underlying_char_t<T>>> str_splitv(
        const T& str,
        const TDelim& delim,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        return detail::str_split<true>(std::basic_string_view(str), delim, offset, maxlen);
    }

    template<typename T, typename TOcurrence>
        requires detail::is_same_underlying_char_type<T, TOcurrence>
    constexpr std::optional<size_t> str_index_of(
        const T& str,
        const TOcurrence& ocurrence,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        using char_type = underlying_char_t<T>;
        std::basic_string_view<char_type> view = detail::str_view_range(str, offset, maxlen);
        size_t index = view.find(ocurrence);
        if (index == decltype(view)::npos)
        {
            return std::nullopt;
        }
        return index;
    }

    template<typename T, typename TOcurrence>
        requires detail::is_same_underlying_char_type<T, TOcurrence>
    constexpr bool str_contains(
        const T& str,
        const TOcurrence& ocurrence,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        return str_index_of(str, ocurrence, offset, maxlen).has_value();
    }

    template<concepts::StdAnyStrStrV T, concepts::AnyChar TOcurrence, concepts::AnyChar TReplacement>
        requires detail::is_same_underlying_char_type<T, TOcurrence, TReplacement>
    constexpr std::basic_string<underlying_char_t<T>> str_replace(
        const T& str,
        const TOcurrence& ocurrence,
        const TReplacement& replacement,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        using char_type = underlying_char_t<T>;
        std::basic_string_view<char_type> view = detail::str_view_range(str, offset, maxlen);
        std::vector<size_t> found_indices = detail::str_find_ocurrence_indices(view, ocurrence);
        std::basic_string result(view.begin(), view.end());
        for(const auto& index : found_indices)
        {
            result[index] = replacement;
        }
        return result;
    }

    template<concepts::AnyStr T, concepts::AnyStrOrChar TOcurrence, concepts::AnyStrOrChar TReplacement>
        requires (detail::is_same_underlying_char_type<T, TOcurrence, TReplacement>
            && concepts::AnyStr<TOcurrence> || concepts::AnyStr<TReplacement>)
    constexpr std::basic_string<underlying_char_t<T>> str_replace(
        const T& str,
        const TOcurrence& ocurrence,
        const TReplacement& replacement,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        using char_type = underlying_char_t<T>;

        const size_t ocurrence_length = detail::str_length(ocurrence);
        const size_t replacement_length = detail::str_length(replacement);
        const int64_t size_diff = (int64_t)replacement_length - (int64_t)ocurrence_length;

        std::basic_string_view<char_type> view = detail::str_view_range(str, offset, maxlen);
        std::vector<size_t> found_indices = detail::str_find_ocurrence_indices(view, ocurrence);
        std::basic_string result(view.begin(), view.end());

        int64_t replace_offset = 0;

        size_t replacement_len = detail::str_length(replacement);
        const char_type* replacement_data = detail::str_dataptr(replacement);
        const std::basic_string_view<char_type> replacement_view(replacement_data, replacement_len);

        for(const auto& index : found_indices)
        {
            result.replace(index + replace_offset, ocurrence_length, replacement_view);
            replace_offset += size_diff;
        }
        return result;
    }

#ifdef IEN_OS_WIN
    extern std::string wstr_to_str(std::wstring_view wstr);
    extern std::wstring str_to_wstr(std::string_view wstr);
    extern std::wstring u8str_to_wstr(std::u8string_view wstr);
#endif

    inline std::string u8str_to_str(std::u8string_view u8str)
    {
        return std::string(u8str.begin(), u8str.end());
    }

    inline std::u8string str_to_u8str(std::string_view str)
    {
        return std::u8string(str.begin(), str.end());
    }

    template<typename TString>
    std::wstring xstr_to_wstr(const TString& str)
    {
        if constexpr (std::is_same_v<typename TString::value_type, char>)
        {
            return str_to_wstr(str);
        }
        else if constexpr (std::is_same_v<typename TString::value_type, char8_t>)
        {
            return u8str_to_wstr(str);
        }
        else if constexpr (std::is_same_v<typename TString::value_type, wchar_t>)
        {
            return std::wstring(str);
        }
        return { };
    }

    template<concepts::AnyStr TString>
    std::string xstr_to_str(const TString& str)
    {
        if constexpr (concepts::StdAnyStrStrV<TString>)
        {
            using char_type = typename TString::value_type;
            if constexpr (std::is_same_v<char_type, char>)
            {
                return std::string(str);
            }
            else if constexpr (std::is_same_v<char_type, char8_t>)
            {
                return u8str_to_str(str);
            }
            else if constexpr (std::is_same_v<char_type, wchar_t>)
            {
                return wstr_to_str(str);
            }
        }
        else if constexpr (concepts::RawAnyStr<TString>)
        {
            using char_type = raw_str_char_t<TString>;
            if constexpr (std::is_same_v<char_type, char>)
            {
                return std::string(str);
            }
            else if constexpr (std::is_same_v<char_type, char8_t>)
            {
                return u8str_to_str(str);
            }
            else if constexpr (std::is_same_v<char_type, wchar_t>)
            {
                return wstr_to_str(str);
            }
        }
    }
}