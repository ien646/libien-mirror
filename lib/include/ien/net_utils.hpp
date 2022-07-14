#pragma once

#include <ien/platform.hpp>

#include <optional>
#include <string>

#ifdef IEN_OS_WIN
    #include <basetsd.h>
#endif

namespace ien
{
    #ifdef IEN_OS_WIN
        using SOCKET = UINT_PTR;
        using socket_t = SOCKET;
    #else
        using socket_t = int;
    #endif

    extern bool is_ipv4_address(const std::string& str);
    extern bool is_ipv6_address(const std::string& str);
    extern std::optional<std::string> resolve_hostname_ipv4(const std::string& host);
    extern std::optional<std::string> resolve_hostname_ipv6(const std::string& host);
}
