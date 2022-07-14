#pragma once

#include <ien/bits/str_utils_defs.hpp>

#include <limits>

namespace ien::detail
{
    template<bool StrView, concepts::AnyStr T, concepts::AnyStrOrChar TDelim>
        requires is_same_underlying_char_type<T, TDelim>
    std::vector<detail::str_or_strview<StrView, underlying_char_t<T>>> str_split(
        const T& str,
        const TDelim& delim,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        using char_type = underlying_char_t<T>;
        using segment_type = str_or_strview<StrView, char_type>;

        size_t str_len = str_length(str);

        if (str_len == 0) { return {}; }

        size_t delim_len = str_length(delim);
        maxlen = std::min(str_len, maxlen);

        std::basic_string_view<char_type> view = str_view_range(str, offset, maxlen);

        std::vector<size_t> found_indices = str_find_ocurrence_indices(view, delim);

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