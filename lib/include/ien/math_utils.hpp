#pragma once

#include <ien/lang_utils.hpp>

namespace ien
{
    constexpr float remap(float value, float source_start, float source_end, float output_start, float output_end)
    {
        return output_start + (output_end - output_start) * ((value - source_start) / (source_end - source_start));
    }

    constexpr double remapd(double value, double source_start, double source_end, double output_start, double output_end)
    {
        return output_start + (output_end - output_start) * ((value - source_start) / (source_end - source_start));
    }
} // namespace ien