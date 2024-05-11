#pragma once

#include <ien/lang_utils.hpp>
#include <ien/bits/str_utils/common.hpp>

namespace ien
{
    namespace detail
    {
        template<concepts::StdAnyStrStrV T, typename Op>
        std::basic_string<ien::underlying_char_t<T>> str_tox(const T& str, Op op)
        {
            std::basic_string<ien::underlying_char_t<T>> result;
            result.resize(str.size());
            std::transform(str.cbegin(), str.cend(), result.begin(), op);
            return result;
        }
    }

    template<concepts::StdAnyStrStrV T>
    std::basic_string<ien::underlying_char_t<T>> str_tolower(const T& str)
    {
        return detail::str_tox(str, static_cast<int(*)(int)>(std::tolower));
    }

    template<concepts::StdAnyStrStrV T>
    std::basic_string<ien::underlying_char_t<T>> str_toupper(const T& str)
    {
        return detail::str_tox(str, static_cast<int(*)(int)>(std::toupper));
    }
}