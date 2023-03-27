#pragma once

#include <ien/lang_utils.hpp>
#include <ien/platform.hpp>

#include <cstddef>
#include <string>

namespace ien::detail
{
    template<concepts::AnyStrOrChar T, concepts::AnyStrOrChar ... TOther>
    constexpr bool is_same_underlying_char_type =
        (std::is_same_v<underlying_char_t<T>, underlying_char_t<TOther>> && ...);

    template<concepts::AnyStrOrChar T>
    IEN_CPP_STDSTRING_CONSTEXPR std::basic_string_view<underlying_char_t<T>> str_view_range(const T& str, size_t offset, size_t maxlen)
    {
        using char_type = underlying_char_t<T>;
        return std::basic_string_view<char_type>(
            anystr_data(str) + offset,
            std::min(anystr_length(str) - offset, maxlen)
        );
    }
}