#pragma once

#include <ien/lang_utils.hpp>

namespace ien
{
    template <
        concepts::Arithmetic TValue,
        concepts::Arithmetic TRange0,
        concepts::Arithmetic TRange1,
        concepts::Arithmetic TRange2,
        concepts::Arithmetic TRange3>
        requires(ien::all_convertible_to_v<TValue, TRange0, TRange1, TRange2, TRange3>)
    TValue remap(TValue value, TRange0 source_start, TRange1 source_end, TRange2 output_start, TRange3 output_end)
    {
        return output_start + (output_end - output_start) * ((value - source_start) / (source_end - source_start));
    }
} // namespace ien