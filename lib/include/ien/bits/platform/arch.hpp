#pragma once

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