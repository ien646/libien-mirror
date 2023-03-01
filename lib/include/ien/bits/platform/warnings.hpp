#pragma once

#include <ien/bits/platform/arch.hpp>

#ifdef IEN_COMPILER_MSVC
    #define IEN_WARNINGS_DISABLE_BEGIN _Pragma("warning(push, 0)")
    #define IEN_WARNINGS_DISABLE_END _Pragma("warning(pop)")
#else
    #define IEN_WARNINGS_DISABLE_BEGIN \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wall\"")
    #define IEN_WARNINGS_DISABLE_END _Pragma("GCC diagnostic pop")
#endif