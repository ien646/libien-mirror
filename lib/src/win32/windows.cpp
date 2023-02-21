#include <ien/platform.hpp>
#include <string>

#ifdef IEN_OS_WIN

#include <ien/win32/windows.h>
#include <filesystem>
#include <shellapi.h>

namespace ien::win32
{
    std::vector<std::wstring> get_cmdline_wargs()
    {
        std::vector<std::wstring> result;

        int w_argc = 0;
        auto cmdline = GetCommandLineW();
        LPWSTR* w_argv = CommandLineToArgvW(cmdline, &w_argc);

        for(int i = 0; i < w_argc; ++i)
        {
            result.push_back(std::wstring(w_argv[i]));
        }
        LocalFree(w_argv);
        return result;
    }
}

#endif