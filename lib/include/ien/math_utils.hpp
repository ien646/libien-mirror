#pragma once

#include <ien/lang_utils.hpp>

namespace ien
{
    template <typename TValue, typename TRange0, typename TRange1, typename TRange2, typename TRange3>
        requires(ien::all_convertible_to_v<TValue, TRange0, TRange1, TRange2, TRange3>)
    TValue remap(TValue value, TRange0 source_start, TRange1 source_end, TRange2 output_start, TRange3 output_end)
    {
        return source_start + (output_end - output_start) * ((value - source_start) / (source_end - source_start));
    }
} // namespace ien