#pragma once

#include <ien/lang_utils.hpp>
#include <ien/bits/str_utils/common.hpp>

namespace ien
{
    template<size_t ExpectedHits = 0, concepts::AnyStr TStr, concepts::AnyStrOrChar TOcurrence>
        requires detail::is_same_underlying_char_type<TStr, TOcurrence>
    IEN_CPP_STDVECTOR_CONSTEXPR std::vector<size_t> str_indices_of(const TStr& str, const TOcurrence& ocurrence, size_t offset = 0)
    {
        const size_t ocurrence_len = anystr_length(ocurrence);
        std::vector<size_t> found_indices;
        if constexpr (ExpectedHits > 0)
        {
            found_indices.reserve(ExpectedHits);
        }
        size_t from_offset = offset;
        from_offset = str.find(ocurrence, from_offset);
        while (from_offset != str.npos)
        {
            found_indices.push_back(from_offset);
            from_offset = str.find(ocurrence, from_offset + ocurrence_len);
        }
        return found_indices;
    };
}