#pragma once

#if defined(WIN64) || defined(_WIN64)
    #define IEN_OS_WIN
    #define IEN_OS_WIN64
#elif defined(WIN32) || defined(_WIN32)
    #define IEN_OS_WIN
    #define IEN_OS_WIN32
#endif

#ifdef IEN_OS_WIN
    #define IEN_OS_WIN_SELECT(win, other) (win)
#else
    #define IEN_OS_WIN_SELECT(win, other) (other)
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

#if defined(__ANDROID__)
    #define IEN_OS_ANDROID
#endif

#if __has_include(<unistd.h>)
    #include <unistd.h>
    #ifdef _POSIX_VERSION
        #define IEN_POSIX
    #endif
#endif
