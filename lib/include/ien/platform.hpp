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
    template <typename T>
    constexpr bool is_ptr_aligned(const T* ptr, size_t alignment)
    {
        if (ptr == nullptr)
        {
            return true;
        }
        const uintptr_t prtval = reinterpret_cast<uintptr_t>(ptr);
        return (prtval % alignment) == 0;
    }
} // namespace ien

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
        ABM,
        ADX,
        AES,
        AVX2,
        AVX512BW,
        AVX512CD,
        AVX512DQ,
        AVX512ER,
        AVX512F,
        AVX512IFMA,
        AVX512PF,
        AVX512VBMI,
        AVX512VL,
        AVX,
        BMI1,
        BMI2,
        FMA3,
        FMA4,
        MMX,
        PREFETCHWT1,
        RDRAND,
        SHA,
        SSE2,
        SSE3,
        SSE41,
        SSE42,
        SSE4a,
        SSE,
        SSSE3,
        x64,
        XOP
    };

    /// @brief Query availability of provided cpu feature
    bool get_feature(feature feat);

    /// @brief Artificially set the availability of the provided cpu feature
    void force_feature(feature feat, bool enabled);
} // namespace ien::platform::x86

#endif

#if defined(IEN_COMPILER_GNU) || defined(IEN_COMPILER_CLANG) || defined(IEN_COMPILER_INTEL)
    #define IEN_PACKED_STRUCT_BEGIN(name) struct __attribute__((__packed__)) name
    #define IEN_PACKED_STRUCT_END()
#elif defined(IEN_COMPILER_MSVC)
    #define IEN_PACKED_STRUCT_BEGIN(name) __pragma(pack(push, 1)) struct name
    #define IEN_PACKED_STRUCT_END() __pragma(pack(pop))
#endif

#ifdef IEN_COMPILER_MSVC
    #define IEN_PUSH_FAST_FLOAT                                                                                        \
        _Pragma("float_control(push)");                                                                                \
        _Pragma("float_control(precise, off)");                                                                        \
        _Pragma("float_control(except, off)");                                                                         \
        _Pragma("fp_contract(on)")

    #define IEN_POP_FAST_FLOAT _Pragma("float_control(pop)")
#else
    #define IEN_PUSH_FAST_FLOAT                                                                                        \
        _Pragma("GCC push_options");                                                                                   \
        _Pragma("GCC optimize (\"fast-math\")")

    #define IEN_POP_FAST_FLOAT _Pragma("GCC pop_options")
#endif