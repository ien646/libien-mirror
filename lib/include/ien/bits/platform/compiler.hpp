#pragma once

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

#if defined(IEN_COMPILER_MSVC)
    #define IEN_HINT_UNREACHABLE() __assume(0)
#elif defined(IEN_COMPILER_GNU) || defined(IEN_COMPILER_CLANG) || defined(IEN_COMPILER_INTEL)
    #define IEN_HINT_UNREACHABLE() __builtin_unreachable()
#else
    #define IEN_HINT_UNREACHABLE()
#endif

#if defined(__MINGW32__)
    #define IEN_MINGW
#endif

