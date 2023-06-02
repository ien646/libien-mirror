#pragma once

#include <ien/lang_utils.hpp>
#include <ien/bits/str_utils/common.hpp>
#include <string>
#include <string_view>

namespace ien
{
    template<concepts::StdAnyStrStrV T>
    std::basic_string<underlying_char_t<T>> str_trim(const T& str, underlying_char_t<T> ch = ' ')
    {
        size_t len = anystr_length(str);
        if(len == 0)
        {
            return std::basic_string<underlying_char_t<T>>{};
        }

        size_t start_index = 0;
        size_t end_index = len - 1;

        while(str[start_index] == ch)
        {
            ++start_index;
        }
        
        if(end_index <= start_index)
        {
            return std::basic_string<underlying_char_t<T>>{};
        }

        while(end_index > start_index && str[end_index] == ch)
        {
            --end_index;
        }

        const underlying_char_t<T>* data = anystr_data(str);
        size_t new_str_len = end_index - start_index + 1;
        return std::basic_string<underlying_char_t<T>>(data + start_index, new_str_len);
    }
}