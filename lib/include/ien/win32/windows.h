#pragma once

#include <ien/platform.hpp>
#include <string>
#include <vector>

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

#include <windows.h>

#undef NOMINMAX
#undef WIN32_NO_STATUS
#undef WIN32_LEAN_AND_MEAN

namespace ien::win32
{
	std::vector<std::wstring> get_cmdline_wargs();
}
