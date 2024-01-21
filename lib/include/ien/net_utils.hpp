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

    bool is_ipv4_address(const std::string& str);
    bool is_ipv6_address(const std::string& str);
    std::optional<std::string> resolve_hostname_ipv4(const std::string& host);
    std::optional<std::string> resolve_hostname_ipv6(const std::string& host);
}
