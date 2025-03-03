#pragma once

namespace ien
{
    enum class resize_filter
    {
        DEFAULT = 0,      // use same filter type that easy-to-use API chooses
        BOX = 1,          // A trapezoid w/1-pixel wide ramps, same result as box for integer scale ratios
        TRIANGLE = 2,     // On upsampling, produces same results as bilinear texture filtering
        CUBICBSPLINE = 3, // The cubic b-spline (aka Mitchell-Netrevalli with B=1,C=0), gaussian-esque
        CATMULLROM = 4,   // An interpolating cubic spline
        MITCHELL = 5,     // Mitchell-Netrevalli filter with B=1/3, C=1/3
        POINT_SAMPLE = 6  // Simple point sampling
    };
}