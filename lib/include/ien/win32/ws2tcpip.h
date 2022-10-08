#pragma once

#include <ien/platform.hpp>

#ifndef IEN_OS_WIN
#error "Unable to use Windows headers outside of Windows"
#endif

#define NOMINMAX
#define WIN32_NO_STATUS
#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#undef NOMINMAX
#undef WIN32_NO_STATUS
#undef WIN32_LEAN_AND_MEAN