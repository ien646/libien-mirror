#pragma once

#include <ien/lang_utils.hpp>

namespace ien
{
    template<concepts::StdAnyStrStrV T, concepts::AnyChar TOcurrence, concepts::AnyChar TReplacement>
        requires detail::is_same_underlying_char_type<T, TOcurrence, TReplacement>
    IEN_CPP_STDSTRING_CONSTEXPR std::basic_string<underlying_char_t<T>> str_replace(
        const T& str,
        const TOcurrence& ocurrence,
        const TReplacement& replacement,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        using char_type = underlying_char_t<T>;
        std::basic_string_view<char_type> view = detail::str_view_range(str, offset, maxlen);
        std::vector<size_t> found_indices = str_indices_of(view, ocurrence);
        std::basic_string result(view.begin(), view.end());
        for(const auto& index : found_indices)
        {
            result[index] = replacement;
        }
        return result;
    }

    template<concepts::AnyStr T, concepts::AnyStrOrChar TOcurrence, concepts::AnyStrOrChar TReplacement>
        requires (detail::is_same_underlying_char_type<T, TOcurrence, TReplacement>
            && concepts::AnyStr<TOcurrence> || concepts::AnyStr<TReplacement>)
    IEN_CPP_STDSTRING_CONSTEXPR std::basic_string<underlying_char_t<T>> str_replace(
        const T& str,
        const TOcurrence& ocurrence,
        const TReplacement& replacement,
        size_t offset = 0,
        size_t maxlen = std::numeric_limits<size_t>::max())
    {
        using char_type = underlying_char_t<T>;

        const size_t ocurrence_length = anystr_length(ocurrence);
        const size_t replacement_length = anystr_length(replacement);
        const int64_t size_diff = (int64_t)replacement_length - (int64_t)ocurrence_length;

        std::basic_string_view<char_type> view = detail::str_view_range(str, offset, maxlen);
        std::vector<size_t> found_indices = str_indices_of(view, ocurrence);
        std::basic_string result(view.begin(), view.end());

        int64_t replace_offset = 0;

        std::basic_string_view<char_type> replacement_view(anystr_data(replacement), anystr_length(replacement));

        for(const auto& index : found_indices)
        {
            result.replace(index + replace_offset, ocurrence_length, replacement_view);
            replace_offset += size_diff;
        }
        return result;
    }
}