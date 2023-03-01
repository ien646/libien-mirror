#pragma once

#include <ien/lang_utils.hpp>
#include <ien/bits/str_utils/common.hpp>

namespace ien
{
    template<concepts::AnyStr TStr, concepts::AnyStrOrChar TOcurrence>
        requires detail::is_same_underlying_char_type<TStr, TOcurrence>
    IEN_CPP_STDVECTOR_CONSTEXPR std::vector<size_t> str_indices_of(const TStr& str, const TOcurrence& ocurrence)
    {
        const size_t ocurrence_len = anystr_length(ocurrence);
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