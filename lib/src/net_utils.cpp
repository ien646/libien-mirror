#include <ien/net_utils.hpp>

#include <ien/platform.hpp>

#include <array>

#ifdef IEN_OS_WIN
	#define NOMINMAX
	#define WIN32_NO_STATUS
	#define WIN32_LEAN_AND_MEAN
	#include <WinSock2.h>
	#include <Ws2tcpip.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
#endif

namespace ien
{
	bool is_ipv4_address(const std::string& str)
	{
		sockaddr_in sa = {};
		return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr));
	}

	bool is_ipv6_address(const std::string& str)
	{
		sockaddr_in6 sa = {};
		return inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr));
	}

	template<bool IpV4>
	std::optional<std::string> _resolve_hostname(const std::string& hostname)
	{
		constexpr auto is_ip_method = IpV4 ? &is_ipv4_address : &is_ipv6_address;
		constexpr auto ai_family = IpV4 ? AF_INET : AF_INET6;
		constexpr auto ai_protocol = IpV4 ? IPPROTO_IP : IPPROTO_IPV6;

		using sockaddr_in_t = std::conditional_t<IpV4, sockaddr_in, sockaddr_in6>;

		if (is_ip_method(hostname))
		{
			return hostname;
		}

		addrinfo* info = nullptr;
		if(int err = getaddrinfo(hostname.c_str(), NULL, NULL, &info))
		{
			return std::nullopt;
		}

		while (info->ai_family != ai_family || info->ai_protocol != ai_protocol)
		{
			info = info->ai_next;
			if (!info)
			{
				break;
			}
		}

		std::optional<std::string> result;
		if (info == nullptr)
		{
			freeaddrinfo(info);
		}
		else
		{
			sockaddr_in_t* addr = reinterpret_cast<sockaddr_in_t*>(info->ai_addr);
			std::array<char, 64> buff = {};
			const char* str = nullptr;
			if constexpr (IpV4)
			{
				str = inet_ntop(ai_family, &addr->sin_addr, buff.data(), buff.size());
			}
			else
			{
				str = inet_ntop(ai_family, &addr->sin6_addr, buff.data(), buff.size());
			}

			if(str)
			{
				result = str;
			}
			else
			{
				result = std::nullopt;
			}
			freeaddrinfo(info);
		}
		return result;
	}

    std::optional<std::string> resolve_hostname_ipv4(const std::string& host)
	{
		return _resolve_hostname<true>(host);
	}

	std::optional<std::string> resolve_hostname_ipv6(const std::string& host)
	{
		return _resolve_hostname<false>(host);
	}
}