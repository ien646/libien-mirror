#pragma once

#include <ien/bits/platform/arch.hpp>

#if defined(IEN_ARCH_ARM) || defined(IEN_ARCH_ARM64)
    #if defined(__ARM_NEON) || defined(__ARM_NEON__)
        #ifndef IEN_ARM_NEON
            #define IEN_ARM_NEON
        #endif
    #endif
#endif