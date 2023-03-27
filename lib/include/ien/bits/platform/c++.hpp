#pragma once

#include <ien/bits/platform/compiler.hpp>

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

// -- constexpr std::vector/std::string --
#ifdef IEN_HAS_CPP20
    #if (defined(IEN_COMPILER_MSVC) && IEN_COMPILER_MSVC_VER >= 1930) \
    || (defined(IEN_COMPILER_GNU) && IEN_COMPILER_GNU_VER >= 12) \
    || (defined(IEN_COMPILER_CLANG) && IEN_COMPILER_CLANG_VER >= 15)
        #define IEN_CPP_STDVECTOR_CONSTEXPR constexpr
        #define IEN_CPP_STDSTRING_CONSTEXPR constexpr
    #endif
#else
    #define IEN_CPP_STDVECTOR_CONSTEXPR
    #define IEN_CPP_STDSTRING_CONSTEXPR
#endif