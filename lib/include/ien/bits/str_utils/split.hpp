#pragma once

#include <ien/bits/str_utils/common.hpp>

#include <limits>

namespace ien::detail
{
    template<bool StrView, concepts::AnyStr T, concepts::AnyStrOrChar TDelim>
        requires is_same_underlying_char_type<T, TDelim>
    IEN_CPP_STDVECTOR_CONSTEXPR std::vector<std::conditional_t<StrView,
                                   std::basic_string_view<underlying_char_t<T>>,
                                   std::basic_string<underlying_char_t<T>>>>
    str_split(
        const T& str,
        const TDelim& delim,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        using char_type = underlying_char_t<T>;
        using segment_type = std::conditional_t<
            StrView,
            std::basic_string_view<underlying_char_t<T>>,
            std::basic_string<underlying_char_t<T>>
        >;

        size_t str_len = anystr_length(str);

        if (str_len == 0) { return {}; }

        size_t delim_len = anystr_length(delim);
        maxlen = std::min(str_len, maxlen);

        std::basic_string_view<char_type> view = str_view_range(str, offset, maxlen);

        std::vector<size_t> found_indices = str_indices_of(view, delim);

        std::vector<segment_type> result;
        result.reserve(found_indices.size());

        const auto begin = std::cbegin(view);
        const auto end = std::cend(view);

        result.push_back(segment_type(begin, begin + found_indices[0]));
        for (size_t i = 0; i < found_indices.size() - 1; ++i)
        {
            const size_t from = found_indices[i] + delim_len;
            const size_t to = found_indices[i + 1];

            if (from == to) { continue; }

            result.push_back(segment_type(begin + from, begin + to));
        }
        if (found_indices.back() != view.size() - 1)
        {
            result.push_back(segment_type(begin + found_indices.back() + delim_len, end));
        }

        return result;
    }
}

namespace ien
{
    template<concepts::AnyStr T, concepts::AnyStrOrChar TDelim>
        requires detail::is_same_underlying_char_type<T, TDelim>
    IEN_CPP_STDVECTOR_CONSTEXPR std::vector<std::basic_string<underlying_char_t<T>>> str_split(
        const T& str,
        const TDelim& delim,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        return detail::str_split<false>(str, delim, offset, maxlen);
    }

    template<concepts::AnyStr T, concepts::AnyStrOrChar TDelim>
        requires detail::is_same_underlying_char_type<T, TDelim>
    IEN_CPP_STDVECTOR_CONSTEXPR std::vector<std::basic_string_view<underlying_char_t<T>>> str_splitv(
        const T& str,
        const TDelim& delim,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        return detail::str_split<true>(std::basic_string_view(str), delim, offset, maxlen);
    }
}