#pragma once

#include <ien/lang_utils.hpp>

namespace ien
{
    template <
        concepts::Arithmetic TValue,
        concepts::Arithmetic TRange0,
        concepts::Arithmetic TRange1,
        concepts::Arithmetic TRange2,
        concepts::Arithmetic TRange3,
        concepts::FloatingPoint TIntermediate = double>
        requires(ien::all_convertible_to_v<TValue, TRange0, TRange1, TRange2, TRange3>)
    TValue remap(TValue value, TRange0 source_start, TRange1 source_end, TRange2 output_start, TRange3 output_end)
    {
        const auto val = static_cast<TIntermediate>(value);
        const auto src_st = static_cast<TIntermediate>(source_start);
        const auto src_en = static_cast<TIntermediate>(source_end);
        const auto out_st = static_cast<TIntermediate>(output_start);
        const auto out_en = static_cast<TIntermediate>(output_end);

        return out_st + (out_en - out_st) * ((val - src_st) / (src_en - src_st));
    }
} // namespace ien