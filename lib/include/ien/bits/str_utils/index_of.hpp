#pragma once

#include <ien/lang_utils.hpp>
#include <ien/bits/str_utils/common.hpp>

#include <cinttypes>
#include <optional>

namespace ien
{
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
}