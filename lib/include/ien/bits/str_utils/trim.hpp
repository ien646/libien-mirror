#pragma once

#include <ien/bits/str_utils/common.hpp>
#include <ien/lang_utils.hpp>
#include <string>
#include <string_view>

namespace ien
{
    namespace detail
    {
        template <concepts::StdAnyStrStrV T>
        size_t str_trim_get_left_index(const T& str, underlying_char_t<T> ch, size_t strlen)
        {
            size_t result = 0;
            while (result < strlen && str[result] == ch)
            {
                ++result;
            }
            return result;
        }

        template <concepts::StdAnyStrStrV T>
        size_t str_trim_get_right_index(const T& str, underlying_char_t<T> ch, size_t strlen)
        {
            size_t result = strlen - 1;
            while (result > 0 && str[result] == ch)
            {
                --result;
            }
            return result;
        }
    }

    template <concepts::StdAnyStrStrV T>
    std::basic_string<underlying_char_t<T>> str_trim(const T& str, underlying_char_t<T> ch = ' ')
    {
        const size_t len = anystr_length(str);
        if (len == 0)
        {
            return {};
        }

        const size_t start_index = detail::str_trim_get_left_index(str, ch, len);
        if(start_index >= (len - 1))
        {
            return {};
        }

        const size_t end_index = detail::str_trim_get_right_index(str, ch, len);
        if(end_index == 0)
        {
            return {};
        }       

        if(start_index == end_index)
        {
            return {};
        }

        const size_t new_str_len = end_index - start_index + 1;
        return { anystr_data(str) + start_index, new_str_len };
    }

    template <concepts::StdAnyStrStrV T>
    std::basic_string<underlying_char_t<T>> str_ltrim(const T& str, underlying_char_t<T> ch = ' ')
    {
        const size_t len = anystr_length(str);
        if (len == 0)
        {
            return {};
        }

        const size_t start_index = detail::str_trim_get_left_index(str, ch, len);
        if(start_index >= (len - 1))
        {
            return {};
        }

        const size_t new_str_len = len - start_index;
        return { anystr_data(str) + start_index, new_str_len };
    }

    template <concepts::StdAnyStrStrV T>
    std::basic_string<underlying_char_t<T>> str_rtrim(const T& str, underlying_char_t<T> ch = ' ')
    {
        const size_t len = anystr_length(str);
        if (len == 0)
        {
            return {};
        }

        const size_t end_index = detail::str_trim_get_right_index(str, ch, len);
        if(end_index == 0)
        {
            return {};
        }

        return { anystr_data(str), end_index + 1 };
    }
} // namespace ien