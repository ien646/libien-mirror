#pragma once

//--------------------------------------------
// ARCH
//--------------------------------------------
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64)
    #define IEN_ARCH_X86_64
#endif

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #define IEN_ARCH_X86
#endif

#if defined(__arm__) || defined(_M_ARM)
    #define IEN_ARCH_ARM
#endif

#if defined(__aarch64__)
    #define IEN_ARCH_ARM64
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// OS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if defined(WIN64) || defined(_WIN64)
    #define IEN_OS_WIN
    #define IEN_OS_WIN64
#elif defined(WIN32) || defined(_WIN32)
    #define IEN_OS_WIN
    #define IEN_OS_WIN32
#endif

#if defined(__unix__)
    #define IEN_OS_UNIX
#endif

#if defined(__APPLE__)
    #define IEN_OS_MAC
#endif

#if defined(__linux__)
    #define IEN_OS_LINUX
#endif

#if defined(__FreeBSD__)
    #define IEN_OS_FREEBSD
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// COMPILER
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#if defined(_MSC_VER)
    #define IEN_COMPILER_MSVC
    #define IEN_COMPILER_MSVC_VER _MSC_VER

#elif defined(__GNUC__) && !defined(__clang__)
    #define IEN_COMPILER_GNU
    #define IEN_COMPILER_GNU_VER __GNUC__

#elif defined(__clang__)
    #define IEN_COMPILER_CLANG
    #define IEN_COMPILER_CLANG_VER __clang_major__

#elif defined(__INTEL_COMPILER)
    #define IEN_COMPILER_INTEL
    #define IEN_COMPILER_INTEL_VER __INTEL_COMPILER

#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ARM SPECIFIC
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if defined(IEN_ARCH_ARM) || defined(IEN_ARCH_ARM64)
    #if defined(__ARM_NEON) || defined(__ARM_NEON__)
        #ifndef IEN_ARM_NEON
            #define IEN_ARM_NEON
        #endif
    #endif
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ALIGNMENT
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

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

    template<typename T>
    constexpr T aligned_sz(T sz, size_t alignment)
    {
        return (sz - (sz % alignment) + alignment);
    }
}

#if defined(IEN_ARCH_X86_64) || defined(IEN_ARCH_X86)
    #define IEN_DEFAULT_ALIGNMENT 32
#elif defined(IEN_ARCH_ARM) || defined(IEN_ARCH_ARM64)
    #define IEN_DEFAULT_ALIGNMENT 16
#else
    #define IEN_DEFAULT_ALIGNMENT 32
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// COMPILER HINTS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// -- Unreachable case --
#if defined(IEN_COMPILER_MSVC)
    #define IEN_HINT_UNREACHABLE() __assume(0)
#elif defined(IEN_COMPILER_GNU) || defined(IEN_COMPILER_CLANG) || defined(IEN_COMPILER_INTEL)
    #define IEN_HINT_UNREACHABLE() __builtin_unreachable()
#else
    #define IEN_HINT_UNREACHABLE()
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// C++ VERSION
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#if __cplusplus >= 199711L
    #define IEN_HAS_CPP03
#endif
#if __cplusplus >= 201103L
    #define IEN_HAS_CPP11
#endif
#if __cplusplus >= 201402L
    #define IEN_HAS_CPP14
#endif
#if __cplusplus >= 201703L
    #define IEN_HAS_CPP17
#endif
#if __cplusplus >= 202002L
    #define IEN_HAS_CPP20
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PLATFORM FEATURES (x86)
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#if defined(IEN_ARCH_X86) || defined(IEN_ARCH_X86_64)

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

    extern void force_feature(feature, bool);
    extern bool get_feature(feature);
}

#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WARNINGS
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\

#ifdef IEN_COMPILER_MSVC
    #define IEN_WARNINGS_DISABLE_BEGIN _Pragma("warning(push, 0)")
    #define IEN_WARNINGS_DISABLE_END _Pragma("warning(pop)")
#else
    #define IEN_WARNINGS_DISABLE_BEGIN \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wall\"")
    #define IEN_WARNINGS_DISABLE_END _Pragma("GCC diagnostic pop")
#endif