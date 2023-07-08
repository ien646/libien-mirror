#pragma once

#include <ien/platform.hpp>

#ifndef IEN_OS_WIN
#error "Unable to use Windows headers outside of Windows"
#endif

#ifndef NOMINMAX
    #define NOMINMAX
#endif

#ifndef WIN32_NO_STATUS
    #define WIN32_NO_STATUS
#endif

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#include <ws2tcpip.h>

#undef NOMINMAX
#undef WIN32_NO_STATUS
#undef WIN32_LEAN_AND_MEAN
