#pragma once

#include <ien/lang_utils.hpp>
#include <ien/bits/str_utils/common.hpp>

namespace ien
{
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
}