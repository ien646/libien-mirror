#pragma once

#include <ien/bits/platform/arch.hpp>
#include <ien/bits/platform/arm.hpp>
#include <ien/bits/platform/c++.hpp>
#include <ien/bits/platform/compiler.hpp>
#include <ien/bits/platform/os.hpp>
#include <ien/bits/platform/warnings.hpp>

#include <cstddef>
#include <cstdint>

namespace ien
{
    template<typename T>
    constexpr bool is_ptr_aligned(const T* ptr, size_t alignment)
    {
        if (ptr == nullptr)
        {
            return true;
        }
        const uintptr_t prtval = reinterpret_cast<uintptr_t>(ptr);
        return (prtval % alignment) == 0;
    }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PLATFORM FEATURES (x86)
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#if defined(IEN_ARCH_X86) || defined(IEN_ARCH_X86_64)

#define IEN_SSE_ALIGNMENT 16
#define IEN_AVX_ALIGNMENT 32
#define IEN_SSE_VECSIZE 32
#define IEN_AVX_VECSIZE 64

#include <iostream>
namespace ien::platform::x86
{
    enum class feature : int
    {
        ABM, ADX,
        AES, AVX2,
        AVX512BW, AVX512CD,
        AVX512DQ, AVX512ER,
        AVX512F, AVX512IFMA,
        AVX512PF, AVX512VBMI,
        AVX512VL, AVX,
        BMI1, BMI2,
        FMA3, FMA4,
        MMX, PREFETCHWT1,
        RDRAND, SHA,
        SSE2, SSE3,
        SSE41, SSE42,
        SSE4a, SSE,
        SSSE3, x64,
        XOP
    };

    /// @brief Query availability of provided cpu feature
    extern bool get_feature(feature feat);

    /// @brief Artificially set the availability of the provided cpu feature
    extern void force_feature(feature feat, bool enabled);
}

#endif